#pragma once

#include "Fastboi.h"

using namespace Fastboi;

namespace Fastboi::Components {
    struct SpriteRenderer;
}

namespace Adventure {
    struct RoomChangeEvent;
    struct Room;

    struct Item {
        private:
        inline static std::vector<Item*> globalItems;

        public:
        GORef go;
        Input::KeyListener releaseLsn{SDL_SCANCODE_SPACE};
        const Room* room = nullptr;
        bool isHeld = false;

        Item(GORef&& go);

        void Start();
        
        void Release();
        void TryPickup(const CollisionEvent& e);

        void ReleasePressed(const KeyEvent& e);
        void Collision(const CollisionEvent& e);
        void RoomChanged(const RoomChangeEvent& e);

        inline static const std::vector<Item*>& GetAllItems() { return globalItems; };
    };
    
    enum class KeyColor {
        GOLD = 0,
        WHITE = 1,
        BLACK = 2
    };

    struct Key {
        using Reqs = RequiredComponents<Components::SpriteRenderer, Transform>;

        Fastboi::GORef go;
        const KeyColor color;

        Key(GORef&& go, KeyColor color);

        void Start();

        static void Inst(Gameobject& go, const Position& pos, KeyColor color);
    };

    struct Sword {
        static void Inst(Gameobject& go, const Position& pos);
    };
}