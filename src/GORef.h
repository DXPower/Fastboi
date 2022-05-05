#pragma once

#include <utility>

namespace Fastboi {
    namespace detail {
        struct ComponentBase;
    }

    struct Gameobject;

    class GORef final{
        public:
        Gameobject* go = nullptr;
        detail::ComponentBase* owningComp = nullptr;

        public:
        GORef() = default;
        GORef(Gameobject& go);
        GORef(Gameobject& go, detail::ComponentBase& owner);
        GORef(const GORef& copy);
        GORef(const GORef&& mv);
        ~GORef();

        GORef& operator=(const GORef&& mv) = delete;
        GORef& operator=(Gameobject& rebind);

        // inline const Gameobject& operator()() const { return *go; };
        inline Gameobject& operator()() const { return *go; };

        template<typename T, typename... Args>
        friend struct Component;
        
        friend struct Gameobject;
    };
}