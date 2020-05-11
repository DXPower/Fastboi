#pragma once

#include <utility>

namespace Fastboi {
    struct ComponentBase;
    struct Gameobject;

    class GORef final{
        public:
        Gameobject* go = nullptr;
        ComponentBase* owningComp = nullptr;

        public:
        GORef() = default;
        GORef(Gameobject& go);
        GORef(Gameobject& go, ComponentBase& owner);
        GORef(const GORef& copy);
        GORef(const GORef&& mv);
        ~GORef();

        GORef& operator=(const GORef&& mv) = delete;
        GORef& operator=(Gameobject& rebind);

        // inline const Gameobject& operator()() const { return *go; };
        inline Gameobject& operator()() const { return *go; };

        template<typename T, typename... Args>
        friend class Component;
        
        friend class Gameobject;
    };
}