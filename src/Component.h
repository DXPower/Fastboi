#pragma once

#include <cstddef>
#include "ctti/type_id.hpp"
#include <GORef.h>
#include <functional>
#include <memory>
#include <typeindex>
#include "Utility.h"


#define GenHasFunction(CheckerName, CheckFunction) \
template <typename _T> \
class CheckerName \
{ \
    private: \
    struct yes { }; \
    struct no { }; \
 \
    template <typename C> static yes test(decltype(&C::CheckFunction)) ; \
    template <typename C> static no test(...); \
 \
    public: \
    enum { value = static_cast<int>(std::is_same_v<decltype(test<_T>(0)), yes>) }; \
}

#define HasFunction(CheckerName, CheckType) CheckerName<CheckType>::value

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

    template<class Component_t, typename ... Args>
    struct Component final : ComponentBase {
        std::unique_ptr<Component_t> component;

        Component(Gameobject& go, Args&&... args) : ComponentBase(ctti::type_id<Component_t>()) { 
            component = std::make_unique<Component_t>(GORef(go, *this), std::forward<Args>(args)...);
        };

        Component() : ComponentBase(ctti::type_id<Component_t>()), component(nullptr) { };

        ~Component() = default;

        Component& operator=(const Component&& copy) {
            component = std::move(copy.component);
        }

        Component& operator=(const Component& copy) = delete;

        // Checker functions so we know whether we can call Update or Start on the component
        GenHasFunction(HasUpdate, Update);
        GenHasFunction(HasStart, Start);
        // GenHasFunction(HasDuplicate, Duplicate);

        void Start() override {
            if constexpr (HasStart<Component_t>::value) {
                component->Start();
            } else {
                ; // Noop
            }
        };

        void Update() override { 
            if constexpr (HasUpdate<Component_t>::value) {
                if (enabled)
                    component->Update();
            } else {
                ; // Noop
            }
        };

        void Duplicate(ComponentBase& out, Gameobject* parent = nullptr) const override {
            isDuplicating = true;
            Component<Component_t>& dup = reinterpret_cast<Component<Component_t>&>(out);

            dup.component.reset(new Component_t(*component));
            
            printf("internalGORef: %p\n", internalGORef);

            if (internalGORef != nullptr) {
                std::ptrdiff_t offset = reinterpret_cast<char*>(internalGORef) - reinterpret_cast<char*>(component.get());
                printf("Distance: %i\n", offset);
                dup.internalGORef = reinterpret_cast<GORef*>(reinterpret_cast<std::uintptr_t>(dup.component.get()) + offset);
                printf("Dup addr: %p, new goref: %p\n", dup.component.get(), dup.internalGORef);
                dup.internalGORef->owningComp = &dup;

                if (parent != nullptr) {
                    printf("Changing parent from %p to %p\n", dup.internalGORef->go, parent);
                    dup.internalGORef->go = parent;
                    printf("out. go ref. go %p, and parent %p: \n", out.internalGORef->go, parent);
                    printf("ADDRESS OF GOREF: %p\n", dup.internalGORef);
                }
            }

            printf("ADDRESS OF DUP: %p, OUT: %p\n", &dup, &out);

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

#undef GenHasFunction
#undef HasFunction