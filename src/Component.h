#pragma once

#include <cstddef>
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
        bool isDuplicating = false;
        GORef* internalGORef = nullptr;


        ComponentBase() : enabled(true) { };
        virtual ~ComponentBase() { };

        virtual void Start() = 0;
        virtual void Update() = 0;
        virtual void Duplicate(ComponentBase& out) = 0;
        
        virtual void* Retrieve() = 0; // void* because we need to be able to return a T from template below
    };

    template<class Component_t, typename ... Args>
    struct Component final : ComponentBase {
        std::unique_ptr<Component_t> component;

        Component(Gameobject& go, Args&&... args) : ComponentBase() { 
            component = std::make_unique<Component_t>(GORef(go, *this), std::forward<Args>(args)...);
        };

        Component() : ComponentBase(), component(nullptr) { };

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

        void Duplicate(ComponentBase& out) override {
            isDuplicating = true;
            Component<Component_t>& dup = reinterpret_cast<Component<Component_t>&>(out);

            dup.component.reset(new Component_t(*component));
            
            std::ptrdiff_t offset = reinterpret_cast<char*>(internalGORef) - reinterpret_cast<char*>(component.get());
            printf("Distance: %i\n", offset);
            dup.internalGORef = reinterpret_cast<GORef*>(dup.component.get() + offset);
            dup.internalGORef->owningComp = &dup;

            isDuplicating = false;
        }

        void* Retrieve() override {
            return (void*) component.get();
        };
    };
};

#undef GenHasFunction
#undef HasFunction