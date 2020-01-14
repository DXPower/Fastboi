#pragma once
#include "Component.h"
#include "Renderer.h"
#include <type_traits>
#include <unordered_map>
#include <memory>
#include "Transform.h"
#include "Vec.h"
#include "Events.h"
#include "FastboiCore.h"

struct Collider;

struct Gameobject {
    private:
    bool isDestroying = false;
    bool isStarted = false;
    bool isDeleted = false;

    public:
    const char* name;
    // Mapping type to components. (Lookup only available at compile time)
    std::unordered_map<std::type_index, std::unique_ptr<ComponentBase>> components;

    Fastboi::Signal<Fastboi::GameobjectDeleteEvent::Signal_t_g> deleteSignal;

    // Transform,  Renderer, Collider are core but act like components, so we store them separately.
    // Additionally, because Collider and Renderer can be base classes, this allows for someone to get the
    // component via base class
    std::unique_ptr<Transform> transform;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Collider> collider;

    Gameobject();
    Gameobject(const char* name);
    virtual ~Gameobject();

    void Start();
    void Update();
    void Destroy();
    
    template<class T, typename... Args>
    decltype(auto) AddComponent(Args&&... args);

    template<class T>
    decltype(auto) GetComponent() const;

    template<class T>
    bool HasComponent();

    template<class T>
    void RemoveComponent();
};

template<class T, typename... Args>
decltype(auto) Gameobject::AddComponent(Args&&... args) {
    using namespace std;

    // Special cases for transform and render due to their special treatmeant in Gameobject:: above
    if constexpr (is_same_v<T, Transform>) {
        transform = make_unique<Transform>(forward<Args>(args)...);
    } else if constexpr (std::is_base_of_v<Renderer, T>) { // Support for multiple Renderers, using is_base_of instead
        renderer = make_unique<T>(forward<Args>(args)...);
    } else if constexpr (std::is_base_of_v<Collider, T>) {
        collider = make_unique<T>(forward<Args>(args)...);
    } else {
        type_index typekey = type_index(typeid(T));

        //. Component<T, Args...> creates a Component that instantiates T, with T's arguments forwarded by forward<Args>
        components.emplace(type_index(typeid(T)), make_unique<Component<T, Args...>>(forward<Args>(args)...));
    }

    return (GetComponent<T>());
}

template<class T>
decltype(auto) Gameobject::GetComponent() const {
    if constexpr (std::is_same_v<T, Transform>) {
        return (*transform); //! Extra parens to make decltype(auto) an lvalue reference
    } else if constexpr (std::is_base_of_v<Renderer, T>) {
        return (*static_cast<T*>(renderer.get())); //! Extra parens to make decltype(auto) an lvalue reference
    } else if constexpr (std::is_base_of_v<Collider, T>) {
        return (*static_cast<T*>(collider.get())); //! Extra parens to make decltype(auto) an lvalue reference
    } else if constexpr (std::is_pointer_v<T>) {
        std::type_index typekey = std::type_index(typeid(typename std::remove_pointer<T>::type)); 
        return static_cast<T>(components.at(typekey)->Retrieve());
    } else {
        std::type_index typekey = std::type_index(typeid(T)); 
        return (*static_cast<T*>(components.at(typekey)->Retrieve())); //! Extra parens to make decltype(auto) an lvalue reference
    }
}


template<class T>
bool Gameobject::HasComponent() {
    if constexpr (std::is_same_v<T, Transform>) {
        return (bool) transform;
    } else if constexpr (std::is_base_of_v<Renderer, T>) {
        return (bool) renderer;
    } else if constexpr (std::is_base_of_v<Collider, T>) {
        return (bool) collider;
    } else {
        std::type_index typekey = std::type_index(typeid(T));
        return components.find(typekey) != components.end();
    }
}

template<class T>
void Gameobject::RemoveComponent() {
    if (isDestroying) return; // This is to prevent component's destroyers from removing components they manage

    if constexpr (std::is_same_v<T, Transform>) {
        transform.reset();
    } else if constexpr (std::is_base_of_v<Renderer, T>) {
        renderer.reset();
    } else if constexpr (std::is_base_of_v<Collider, T>) {
        collider.reset();
    } else {
        std::type_index typekey = std::type_index(typeid(T));
        components.erase(typekey);
    }
}