#pragma once
#include "Collider.h"
#include "Component.h"
#include "Renderer.h"
#include <type_traits>
#include <unordered_map>
#include <memory>
#include "Transform.h"
#include "Vec.h"
#include "Events.h"
#include "FastboiCore.h"

namespace Fastboi {
    struct Gameobject {
        private:
        bool isDestroying = false;
        bool isStarted = false;
        bool isDeleted = false;

        public:
        const char* name;
        // Mapping type to components. (Lookup only available at compile time)
        std::unordered_map<std::type_index, std::unique_ptr<ComponentBase>> components;

        /** @deprecated **/
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
        
        template<class T, typename... Args>
        T& AddComponent(Args&&... args);

        template<class T>
        T& GetComponent() const;

        template<class T>
        bool HasComponent() const;

        template<class T>
        void RemoveComponent();

        private:
        void Destroy();

        friend void Fastboi::Destroy(Gameobject&);
        friend void Fastboi::Tick();
    };

    template<class T, typename... Args>
    T& Gameobject::AddComponent(Args&&... args) {
        using namespace std;

        // Special cases for transform and render due to their special treatmeant in Gameobject:: above
        if constexpr (is_same_v<T, Transform>) {
            transform = make_unique<Transform>(forward<Args>(args)...);
        } else if constexpr (is_base_of_v<Renderer, T>) { // Support for multiple Renderers, using is_base_of instead
            renderer = make_unique<T>(forward<Args>(args)...);
            // renderer->Start();
        } else if constexpr (is_base_of_v<Collider, T>) {
            collider = make_unique<T>(forward<Args>(args)...);
            // collider->Start();
        } else {
            type_index typekey = type_index(typeid(T));

            //. Component<T, Args...> creates a Component that instantiates T, with T's arguments forwarded by forward<Args>
            const auto [it, _] = components.emplace(typekey, make_unique<Component<T, Args...>>(forward<Args>(args)...));
            
            // Run ComponentBase::Start() if the gameobject has already started
            if (isStarted) {
                Fastboi::Print("Auto-starting component due to isStarted == true\n");
                (*it).second->Start();
            }
        }

        return (GetComponent<T>());
    }

    template<class T>
    T& Gameobject::GetComponent() const {
        if constexpr (std::is_same_v<T, Transform>) {
            return *transform;
        } else if constexpr (std::is_base_of_v<Renderer, T>) {
            return *static_cast<T*>(renderer.get());
        } else if constexpr (std::is_base_of_v<Collider, T>) {
            return *static_cast<T*>(collider.get());
        } else {
            std::type_index typekey = std::type_index(typeid(T)); 
            return *static_cast<T*>(components.at(typekey)->Retrieve());
        }
    }

    template<class T>
    bool Gameobject::HasComponent() const {
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
}