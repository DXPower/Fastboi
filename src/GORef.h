#pragma once

#include <utility>

namespace Fastboi {
    struct Gameobject;

    class GORef final{
        Gameobject* go = nullptr;

        public:
        GORef(Gameobject& go);

        GORef& operator=(Gameobject& rebind);

        inline const Gameobject& operator()() const { return *go; };
        inline Gameobject& operator()() { return *go; };
    };
}