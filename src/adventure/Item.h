#pragma once

#include "Fastboi.h"

using namespace Fastboi;

namespace Fastboi::Components {
    struct SpriteRenderer;
}

namespace Adventure {
    struct Item {
        bool isHeld = false;
        GORef go;

        Item(GORef&& go);

        void Start();

        void Collision(const CollisionEvent& e);
    };

    struct Key {
        using Reqs = RequiredComponents<Components::SpriteRenderer, Transform>;

        enum KeyColor {
            GOLD = 0,
            WHITE = 1,
            BLACK = 2
        };

        const KeyColor color;
        Fastboi::GORef go;

        Key(GORef&& go, KeyColor color);

        void Start();
    };

    void KeyGO(Gameobject& go, const Position& pos, Key::KeyColor color);
}