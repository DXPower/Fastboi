#pragma once

#include "Fastboi.h"

using namespace Fastboi;

namespace Fastboi::Components {
    struct SpriteRenderer;
}

namespace Adventure {
    struct Item {
        GORef go;
        Input::KeyListener releaseLsn{SDL_SCANCODE_SPACE};
        bool isHeld = false;
        

        Item(GORef&& go);

        void Start();

        void Collision(const CollisionEvent& e);
        void Release(const KeyEvent& e);
    };

    
    enum class KeyColor {
        GOLD = 0,
        WHITE = 1,
        BLACK = 2
    };

    struct Key {
        using Reqs = RequiredComponents<Components::SpriteRenderer, Transform>;

        const KeyColor color;
        Fastboi::GORef go;

        Key(GORef&& go, KeyColor color);

        void Start();
    };

    void KeyGO(Gameobject& go, const Position& pos, KeyColor color);
}