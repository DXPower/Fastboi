#pragma once
#include "Collider.h"
#include "Component.h"
#include "Events.h"
#include "FastboiCore.h"
#include <memory>
#include <unordered_map>
#include "Renderer.h"
#include "Transform.h"
#include <type_traits>
#include "Vec.h"
#include "VelocityComp.h"

namespace Fastboi {
    struct Gameobject final {
        private:
        bool isDestroying = false;
        bool isStarted = false;
        bool isDeleted = false;
        bool isEnabled = true;

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

        void SetEnabled(bool f);
        inline void Enable() { SetEnabled(true); };
        inline void Disable() { SetEnabled(false); };
        inline bool IsEnabled() const { return isEnabled; };

        template<class T>
        void SetComponentEnabled(bool f);

        template<class T>
        void EnableComponent() { SetComponentEnabled<T>(true); };
        
        template<class T>
        void DisableComponent() { SetComponentEnabled<T>(false); };

        template<class T>
        bool IsComponentEnabled() const;

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

    template<class T>
    void Gameobject::SetComponentEnabled(bool f) {
        static_assert(!std::is_same_v<Transform, T> && !std::is_same_v<Renderer, T>);

        using namespace Fastboi::Components;

        if constexpr (std::is_same_v<Collider, T>) {
            if (collider)
                collider->SetEnabled(f);
            else
                Application::ThrowRuntimeException(Application::COMPONENT_NO_EXIST);
        } else if constexpr (std::is_same_v<VelocityComp, T>) {
            return GetComponent<VelocityComp>().SetEnabled(f);
        } else {
            std::type_index typekey = std::type_index(typeid(T));
            components.at(typekey)->enabled = f;
        }
    }

    template<class T>
    bool Gameobject::IsComponentEnabled() const {
        static_assert(!std::is_same_v<Transform, T> && !std::is_same_v<Renderer, T>);

        if constexpr (std::is_same_v<Collider, T>) {
            if (collider)
                return collider->IsEnabled();
            else
                Application::ThrowRuntimeException(Application::COMPONENT_NO_EXIST);
        } else if constexpr (std::is_same_v<VelocityComp, T>) {
            return GetComponent<VelocityComp>().IsEnabled();
        } else {
            std::type_index typekey = std::type_index(typeid(T));
            return components.at(typekey)->enabled;
        }
    }
}