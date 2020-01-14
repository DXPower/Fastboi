#pragma once

#include <typeindex>
#include <functional>
#include <any>
#include "Utility.h"
#include <memory>

// Base component without any template specializations so it can be stored in a container (Namely Gameobject::components)
struct ComponentBase {
    bool enabled;

    ComponentBase() : enabled(true) { };
    virtual ~ComponentBase() { };

    virtual void Start() { };
    virtual void Update() { };
    
    virtual void* Retrieve() = 0; // void* because we need to be able to return a T from template below
};

template<class Component_t, typename ... Args>
struct Component final : ComponentBase {
    std::unique_ptr<Component_t> component;

    Component(Args... args) { 
        component = std::make_unique<Component_t>(std::forward<Args>(args)...);
    };

    ~Component() { };

    // Checker functions so we know whether we can call Update or Start on the component
    GenHasFunction(HasUpdate, Update);
    GenHasFunction(HasStart, Start);

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

    void* Retrieve() override {
        return (void*) component.get();
    };
};