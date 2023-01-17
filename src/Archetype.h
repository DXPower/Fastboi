#pragma once

#include <tuple>
#include <type_traits>
#include <unordered_map>
#include "Application.h"
#include "Archetype.h"
#include "Component.h"
#include "Exceptions.h"
#include "Gameobject.h"
#include "ctti/type_id.hpp"
#include <iostream>
#include <algorithm>
#include <ranges>

namespace Fastboi {
    struct ArchetypeBase;

    namespace detail {
        template<typename Check, typename... Args>
        constexpr bool is_same_list_v = std::disjunction_v<std::is_same<Check, Args>...>;

        template<typename... List>
        struct IsUnique;

        template<typename Head, typename... Tail>
        struct IsUnique<Head, Tail...>
        {
            static constexpr bool value = !is_same_list_v<Head, Tail...> && IsUnique<Tail...>::value;
        };

        template<>
        struct IsUnique<>
        {
            static constexpr bool value = true ;
        };

        struct ArchetypeEntry {
            ArchetypeBase* archetype;
            std::vector<uint64_t> sortedHashes;
        };

        inline std::vector<ArchetypeEntry> archetypeRegistry;
    };

    template<typename... T>
    constexpr uint64_t TypeListHash_v = (ctti::type_id<T>().hash() ^ ...);

    struct ArchetypeBase {
        uint64_t typelistHash;
        uint64_t uniqueHash;

        using Storage_t = Gameobject*;
        std::vector<Storage_t> gameobjects;

        ArchetypeBase(uint64_t typelistHash, uint64_t uniqueHash) : typelistHash(typelistHash), uniqueHash(uniqueHash) { }
        virtual ~ArchetypeBase() = default;

        virtual bool ContainsComponent(uint64_t hash) const = 0;

        virtual void TickSystems() = 0;
        virtual void* GetSystems() = 0;
    };

    template<typename... T>
    requires detail::IsUnique<T...>::value
    struct Archetype final : ArchetypeBase {
        using System_g = void(Gameobject&, T&...);

        std::vector<std::function<System_g>> systems;

        Archetype() : ArchetypeBase(TypeListHash_v<T...>, ctti::type_id<Archetype<T...>>().hash()) {
            using namespace detail;
            namespace ranges = std::ranges;

            auto insertLocation = ranges::upper_bound(archetypeRegistry, sizeof...(T), {}, [](const ArchetypeEntry& e) {
                return e.sortedHashes.size();
            });

            archetypeRegistry.insert(insertLocation, { this, { ctti::type_id<T>().hash()... } });
            ranges::sort(archetypeRegistry.back().sortedHashes);
        }

        bool ContainsComponent(uint64_t hash) const override {
            return ((hash == ctti::type_id<T>().hash()) || ...);
        }

        void TickSystems() override {
            for (Gameobject* go : gameobjects) {
                std::tuple<Gameobject&, T&...> systemArgs = std::tie(*go, go->GetComponent<T>()...);

                for (const auto& system : systems) {
                    std::apply(system, systemArgs);
                }
            }
        };

        void* GetSystems() override {
            return &systems;
        }
    };

    namespace detail {
        inline std::unordered_map<uint64_t, std::unique_ptr<ArchetypeBase>> archetypes;

        template<auto* F>
        struct RegisterSystemHelper {
            RegisterSystemHelper() = delete;
        };

        template<typename... Args, auto (*F)(Gameobject&, Args...) -> void>
        struct RegisterSystemHelper<F> { 
            using Archetype_t = Archetype<std::remove_cvref_t<Args>...>;

            static inline constexpr auto Func = F;
            static inline constexpr uint64_t typelistHash = TypeListHash_v<std::remove_cvref_t<Args>...>;
            static inline constexpr uint64_t uniqueHash = ctti::type_id<Archetype_t>().hash();
        };
    }

    template<auto F>
    requires std::is_default_constructible_v<detail::RegisterSystemHelper<F>>
    void RegisterSystem() {
        using namespace detail;
        using Helper = RegisterSystemHelper<F>;

        if (not archetypes.contains(Helper::typelistHash))
            archetypes.emplace(Helper::typelistHash, std::make_unique<typename Helper::Archetype_t>());

        auto& base = *archetypes.at(Helper::typelistHash);

        if (base.uniqueHash != Helper::uniqueHash)
            Application::ThrowRuntimeException("Parameters for Systems of the same combination of components must have the same order!", Application::SYSTEM_DIFFERING_PARAMETER_ORDER);        

        typename Helper::Archetype_t& archetype = static_cast<typename Helper::Archetype_t&>(base);

        archetype.systems.emplace_back(F);
    }

    namespace detail {
        inline std::vector<ArchetypeBase*> NotifyNewComponent(Gameobject& go, std::span<const uint64_t> sortedHashes, std::span<ArchetypeBase* const> archetypesAlreadyIn) {
            namespace views = std::views;
            namespace ranges = std::ranges;

            std::vector<ArchetypeBase*> newArchetypes;

            // Skip all entries whose archetype we're already in
            auto filter = [=](const ArchetypeEntry& e) { return ranges::find(archetypesAlreadyIn, e.archetype) == archetypesAlreadyIn.end(); };

            for (const ArchetypeEntry& entry : archetypeRegistry | views::filter(filter)) {
                // The registry is sorted by number of components, so we can stop searching once it's impossible to be a subset
                if (entry.sortedHashes.size() > sortedHashes.size()) break;

                // If this archetype's components is a subset of our own, then we need to be added to this archetype
                if (ranges::includes(sortedHashes, entry.sortedHashes)) {
                    std::cout << "Found archetype!\n";
                    entry.archetype->gameobjects.push_back(&go);
                    newArchetypes.push_back(entry.archetype);
                }
            }

            return newArchetypes;
        }

        inline void NotifyRemovedComponent(Gameobject& go, uint64_t removedHash, std::vector<ArchetypeBase*>& archetypesIn) {
            std::erase_if(archetypesIn, [&](ArchetypeBase* archetype) {
                if (!archetype->ContainsComponent(removedHash))
                    return false;

                std::erase(archetype->gameobjects, &go);
                return true;
            });
        }
    }

    // // These facilities help manage detecting when a component conforms to an archetype by detecting all possible combinations of 
    // // their children components
    // namespace detail {
    //     struct CombinationHelper {
    //         CombinationHelper* inherited;
    //         std::vector<CombinationHelper*> children;

    //         std::vector<uint64_t> combinations;
    //         std::vector<ArchetypeBase*> conformingArchetypes;

    //         inline static std::unordered_map<uint64_t, CombinationHelper> combinationRegistry;

    //         static void NotifyNewComponent(Gameobject& go, uint64_t componentHash, std::optional<uint64_t> oldHash) {
    //             std::cout << "New component added...\n";
    //             if (oldHash.has_value()) {
    //                 // This is not the first component. Find the inherited combinations
    //                 CombinationHelper& inherited = combinationRegistry.at(oldHash.value());

    //                 CombinationHelper comb {
    //                       .inherited = &inherited
    //                     , .combinations = { componentHash }
    //                     , .conformingArchetypes = { }
    //                 };

    //                 // For all inherited combinations, 
    //                 for (CombinationHelper* parent = &inherited; parent != nullptr; parent = parent->inherited) {
    //                     for (uint64_t prevHash : parent->combinations) {
    //                         comb.combinations.push_back(prevHash ^ componentHash);
    //                     }
    //                 }

    //                 for (uint64_t hash : comb.combinations) {
    //                     if (detail::archetypes.contains(hash)) {
    //                         ArchetypeBase& archetype = *detail::archetypes[hash];

    //                         comb.conformingArchetypes.push_back(&archetype);
    //                         archetype.gameobjects.push_back(&go);
    //                     }
    //                 }

    //                 combinationRegistry.emplace(componentHash ^ *oldHash, std::move(comb));
    //             } else {
    //                 // This is the first component for this object. Make a new CombinationHelper
    //                 if (detail::archetypes.contains(componentHash)) {
    //                     ArchetypeBase& archetype = *detail::archetypes[componentHash];

    //                     archetype.gameobjects.push_back(&go);

    //                     combinationRegistry.emplace(componentHash, CombinationHelper {
    //                           .inherited            = nullptr
    //                         , .combinations         = { componentHash }
    //                         , .conformingArchetypes = { &archetype }
    //                     });
    //                 } else {
    //                     combinationRegistry.emplace(componentHash, CombinationHelper {
    //                           .inherited            = nullptr
    //                         , .combinations         = { componentHash }
    //                         , .conformingArchetypes = {  }
    //                     });
    //                 }
    //             }
    //         }

    //         void NotifyRemoveComponentRecurse(Gameobject& go) {
    //             for (ArchetypeBase* archetype : conformingArchetypes) {
    //                 std::erase(archetype->gameobjects, &go);
    //             }

    //             for (CombinationHelper* child : children) {
    //                 child->NotifyRemoveComponentRecurse(go);
    //             }
    //         }

    //         static void NotifyRemoveComponent(Gameobject& go, uint64_t componentHash) {
    //             combinationRegistry.at(componentHash).NotifyRemoveComponentRecurse(go);
    //         }
    //     };
    // }
}