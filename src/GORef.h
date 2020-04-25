#pragma once

#include <utility>

namespace Fastboi {
    struct ComponentBase;
    struct Gameobject;

    class GORef final{
        Gameobject* go = nullptr;
        ComponentBase* owningComp = nullptr;

        public:
        GORef(Gameobject& go);
        GORef(Gameobject& go, ComponentBase& owner);
        GORef(const GORef&& mv);

        GORef& operator=(const GORef&& mv) = delete;
        GORef& operator=(Gameobject& rebind);

        inline const Gameobject& operator()() const { return *go; };
        inline Gameobject& operator()() { return *go; };

        private:
        GORef(const GORef& copy);
    };
}