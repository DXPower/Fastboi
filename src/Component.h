#pragma once

#include <Application.h>
#include <cstddef>
#include "ctti/type_id.hpp"
#include <GORef.h>
#include <functional>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <utility>
#include "Utility.h"

namespace Fastboi {
    namespace detail {
        // Base component without any template specializations so it can be stored in a container (Namely Gameobject::components)
        struct ComponentBase {
            bool enabled;
            const ctti::type_id_t typekey;
            mutable bool isDuplicating = false;
            GORef* internalGORef = nullptr;

            ComponentBase(ctti::type_id_t&& tid) : enabled(true), typekey(std::move(tid)) { };
            virtual ~ComponentBase() { };

            virtual void Start(Gameobject& go) = 0;
            virtual void Update(Gameobject& go) = 0;
            virtual void Duplicate(ComponentBase& out, Gameobject* parent = nullptr) const = 0;
            virtual ComponentBase& CreateEmpty() const = 0; // Creates a ComponentBase with the same size as Component<T>, for use in GO::Duplicate()
            
            virtual void* Retrieve() = 0; // void* because we need to be able to return a T from template below
        };

        template<class Component_t, typename... Args>
        concept HasGOConstructor = std::is_constructible_v<Component_t, GORef&&, Args...>;


        template<class Component_t, typename... Args>
        concept ComponentConstructible = HasGOConstructor<Component_t, Args...> || std::is_constructible_v<Component_t, Args...>;

        template<class Component_t>
        concept HasStart = requires (Component_t c) {
            c.Start();
        };

        template<class Component_t>
        concept HasStartWithGO = requires (Component_t c, Gameobject& go) {
            c.Start(go);
        };

        template<class Component_t>
        concept HasUpdate = requires(Component_t c) {
            c.Update();
        };

        template<class Component_t>
        concept HasUpdateWithGO = requires (Component_t c, Gameobject& go) {
            c.Update(go);
        };

        template<class Component_t>
        concept CanDuplicate = std::is_constructible_v<Component_t, const Component_t&>;
    }

    template<class Component_t, typename... Args>
    struct Component final : detail::ComponentBase {
        std::optional<Component_t> component;

        template<int = 0>
        requires detail::HasGOConstructor<Component_t, Args...>
        Component(Gameobject& go, Args&&... args)
         : ComponentBase(ctti::type_id<Component_t>())
         , component(std::in_place, GORef(go, *this), std::forward<Args>(args)...) {

        };

        template<int = 0>
        requires (!detail::HasGOConstructor<Component_t, Args...>) && detail::ComponentConstructible<Component_t, Args...>
        Component(Gameobject& go, Args&&... args)
         : ComponentBase(ctti::type_id<Component_t>())
         , component(std::in_place, std::forward<Args>(args)...) {

        };

        Component() : ComponentBase(ctti::type_id<Component_t>()), component(std::nullopt) { };

        Component(const Component& copy) = delete;
        Component(Component&& move) = delete;
        Component& operator=(const Component& copy) = delete;
        Component& operator=(Component&& move) = delete;

        void Start(Gameobject& go) override {
            if constexpr (detail::HasStart<Component_t>) {
                component->Start();
            } else if constexpr (detail::HasStartWithGO<Component_t>) {
                component->Start(go);
            }
        };

        void Update(Gameobject& go) override {
            if (!enabled)
                return;

            if constexpr (detail::HasUpdate<Component_t>) {
                component->Update();
            } else if constexpr (detail::HasUpdateWithGO<Component_t>) {
                component->Update(go);
            }
        };

        void Duplicate(ComponentBase& out, Gameobject* parent = nullptr) const override {
            if constexpr(!detail::CanDuplicate<Component_t>)
                Application::ThrowRuntimeException("Can't duplicate component", Application::CANNOT_DUPE, typekey.str().c_str());

            isDuplicating = true;
            Component<Component_t>& dup = reinterpret_cast<Component<Component_t>&>(out);

            dup.component.emplace(component.value());

            if (internalGORef != nullptr) {
                std::ptrdiff_t offset = reinterpret_cast<char*>(internalGORef) - reinterpret_cast<const char*>(&component.value());
                dup.internalGORef = reinterpret_cast<GORef*>(reinterpret_cast<std::uintptr_t>(&dup.component.value()) + offset);
                dup.internalGORef->owningComp = &dup;

                if (parent != nullptr)
                    dup.internalGORef->go = parent;
            }

            isDuplicating = false;
        };

        detail::ComponentBase& CreateEmpty() const override {
            return *(new Component<Component_t>());
        };

        void* Retrieve() override {
            return &component.value();
        };
    };
};