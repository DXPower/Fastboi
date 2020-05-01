#pragma once

#include <cstddef>
#include "ctti/type_id.hpp"
#include <GORef.h>
#include <functional>
#include <memory>
#include <typeindex>
#include <type_traits>
#include "Utility.h"

namespace Fastboi {
    // Base component without any template specializations so it can be stored in a container (Namely Gameobject::components)
    struct ComponentBase {
        bool enabled;
        const ctti::type_id_t typekey;
        mutable bool isDuplicating = false;
        GORef* internalGORef = nullptr;

        ComponentBase(ctti::type_id_t&& tid) : enabled(true), typekey(std::move(tid)) { };
        virtual ~ComponentBase() { };

        virtual void Start() = 0;
        virtual void Update() = 0;
        virtual void Duplicate(ComponentBase& out, Gameobject* parent = nullptr) const = 0;
        virtual ComponentBase& CreateEmpty() const = 0; // Creates a ComponentBase with the same size as Component<T>, for use in GO::Duplicate()
        
        virtual void* Retrieve() = 0; // void* because we need to be able to return a T from template below
    };

    template<class Component_t, typename... Args>
    concept HasGOConstructor = std::is_constructible_v<Component_t, GORef&&, Args...>;

    template<class Component_t>
    concept HasStart = requires (Component_t c) {
        c.Start();
    };

    template<class Component_t>
    concept HasUpdate = requires(Component_t c) {
        c.Update();
    };

    template<class Component_t, typename... Args>
    struct Component final : ComponentBase {
        std::unique_ptr<Component_t> component;

        Component(Gameobject& go, Args&&... args) : ComponentBase(ctti::type_id<Component_t>()) {
            if constexpr (HasGOConstructor<Component_t, Args...>) {
                component = std::make_unique<Component_t>(GORef(go, *this), std::forward<Args>(args)...);
            } else {
                component = std::make_unique<Component_t>(std::forward<Args>(args)...);
            }
        };

        Component() : ComponentBase(ctti::type_id<Component_t>()), component(nullptr) { };

        ~Component() = default;

        Component& operator=(const Component&& copy) {
            component = std::move(copy.component);
        }

        Component& operator=(const Component& copy) = delete;

        void Start() override {
            if constexpr (HasStart<Component_t>) {
                component->Start();
            }
        };

        void Update() override { 
            if constexpr (HasUpdate<Component_t>) {
                if (enabled)
                    component->Update();
            }
        };

        void Duplicate(ComponentBase& out, Gameobject* parent = nullptr) const override {
            isDuplicating = true;
            Component<Component_t>& dup = reinterpret_cast<Component<Component_t>&>(out);

            dup.component.reset(new Component_t(*component));

            if (internalGORef != nullptr) {
                std::ptrdiff_t offset = reinterpret_cast<char*>(internalGORef) - reinterpret_cast<char*>(component.get());
                dup.internalGORef = reinterpret_cast<GORef*>(reinterpret_cast<std::uintptr_t>(dup.component.get()) + offset);
                dup.internalGORef->owningComp = &dup;

                if (parent != nullptr)
                    dup.internalGORef->go = parent;
            }

            isDuplicating = false;
        };

        ComponentBase& CreateEmpty() const override {
            return *(new Component<Component_t>());
        };

        void* Retrieve() override {
            return (void*) component.get();
        };
    };
};