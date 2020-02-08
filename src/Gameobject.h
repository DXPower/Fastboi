#pragma once
#include "Application.h"
#include "ctti/type_id.hpp"
#include "Collider.h"
#include "Component.h"
#include "Events.h"
#include <memory>
#include <unordered_map>
#include "Renderer.h"
#include <stack>
#include "Transform.h"
#include <type_traits>
#include "Vec.h"
#include "VelocityComp.h"

namespace Fastboi {
    void Tick();
    void Destroy(Gameobject&);

    struct Gameobject final {
        private:
        bool isDestroying = false;
        bool isStarted = false;
        bool isDeleted = false;
        bool isEnabled = true;
        bool componentsLock = false;

        uint64_t rendererTypeHash = 0;

        public:
        const char* name;
        std::unordered_map<uint64_t, std::unique_ptr<ComponentBase>> components;
        // Mapping type to components. (Lookup only available at compile time)
        // Transform,  Renderer, Collider are core but act like components, so we store them separately.
        // Additionally, because Collider and Renderer can be base classes, this allows for someone to get the
        // component via base class
        std::unique_ptr<Transform> transform;
        std::unique_ptr<Renderer> renderer;
        std::unique_ptr<Collider> collider;

        Gameobject();
        Gameobject(const char* name);
        Gameobject(const Gameobject& copy) = delete;
        Gameobject(const Gameobject&& copy) = delete;
        virtual ~Gameobject();

        Gameobject& operator=(const Gameobject& copy) = delete;
        Gameobject& operator=(const Gameobject&& copy) = delete;

        void Start();
        void Update();
        
        template<class T, typename... Args>
        T& AddComponent(Args&&... args);

        template<class T>
        T& GetComponent();

        template<class T>
        const T& GetComponent() const;

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
        std::stack<decltype(components)::value_type> componentsToAdd;
        std::stack<decltype(components)::key_type> componentsToRemove;

        void Destroy();
        void AddComponentsOnStack();
        void RemoveComponentsOnStack();

        friend void Fastboi::Tick();
        friend void Fastboi::Destroy(Gameobject&);
    };

    

    template<class T, typename... Args>
    T& Gameobject::AddComponent(Args&&... args) {
        using namespace std;

        // Special cases for transform and render due to their special treatmeant in Gameobject:: above
        if constexpr (is_same_v<T, Transform>) {
            transform = make_unique<Transform>(forward<Args>(args)...);
        } else if constexpr (is_base_of_v<Renderer, T>) {
            renderer = make_unique<T>(*this, forward<Args>(args)...);
            rendererTypeHash = ctti::type_id<T>().hash();
        } else if constexpr (is_same_v<Collider, T>) {
            collider = make_unique<Collider>(*this, forward<Args>(args)...);
        } else {
            constexpr uint64_t typekey = ctti::type_id<T>().hash();

            //. Component<T, Args...> creates a Component that instantiates T, with T's arguments forwarded by forward<Args>
            if (isStarted || componentsLock) {
                componentsToAdd.emplace(typekey, make_unique<Component<T, Args...>>(*this, forward<Args>(args)...));
                return (*static_cast<T*>(componentsToAdd.top().second->Retrieve()));
            } else
                components.emplace(typekey, make_unique<Component<T, Args...>>(*this, forward<Args>(args)...));
        }

        return (GetComponent<T>());
    }

    template<class T>
    T& Gameobject::GetComponent() {
        return const_cast<T&>(const_cast<const Gameobject*>(this)->GetComponent<T>());
    }

    template<class T>
    const T& Gameobject::GetComponent() const {
        if (!HasComponent<T>())
            Application::ThrowRuntimeException("Attempt to get nonexistant component!", 
                Application::COMPONENT_NO_EXIST,
                ctti::type_id<T>().name().str().c_str());
            
        if constexpr (std::is_same_v<T, Transform>) {
            return *transform;
        } else if constexpr (std::is_base_of_v<Renderer, T>) {
            return *static_cast<T*>(renderer.get());
        } else if constexpr (std::is_base_of_v<Collider, T>) {
            return *collider;
        } else {
            constexpr uint64_t typekey = ctti::type_id<T>().hash();
            return *static_cast<T*>(components.at(typekey)->Retrieve());
        }
    }

    template<class T>
    bool Gameobject::HasComponent() const {
        if constexpr (std::is_same_v<T, Transform>) {
            return (bool) transform;
        } else if constexpr (std::is_base_of_v<Renderer, T>) {
            return renderer && rendererTypeHash == ctti::type_id<T>().hash();
        } else if constexpr (std::is_base_of_v<Collider, T>) {
            return (bool) collider;
        } else {
            constexpr uint64_t typekey = ctti::type_id<T>().hash();
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
            rendererTypeHash = 0;
        } else if constexpr (std::is_base_of_v<Collider, T>) {
            collider.reset();
        } else {
            constexpr uint64_t typekey = ctti::type_id<T>().hash();
            componentsToRemove.push(typekey);
        }
    }

    template<class T>
    void Gameobject::SetComponentEnabled(bool f) {
        static_assert(!std::is_same_v<Transform, T> && !std::is_same_v<Renderer, T>);

        if constexpr (std::is_same_v<Collider, T>) {
            if (collider)
                collider->SetEnabled(f);
            else
                Application::ThrowRuntimeException(Application::COMPONENT_NO_EXIST);
        } else if constexpr (std::is_same_v<VelocityComp, T>) {
            return GetComponent<VelocityComp>().SetEnabled(f);
        } else {
            constexpr uint64_t typekey = ctti::type_id<T>().hash();
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
            constexpr uint64_t typekey = ctti::type_id<T>().hash();
            return components.at(typekey)->enabled;
        }
    }
}