#include "Gameobject.h"
#include "Collider.h"

namespace Fastboi {
    void TestPhysicsSystem(Gameobject& go, Collider& col);
}

namespace Adventure {
    struct Key;
    struct Item;

    void TestKeySystem(Fastboi::Gameobject& go, Key& key, Item& item, Fastboi::Transform& transform);
}

