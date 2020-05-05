#pragma once

#include "Fastboi.h"

using namespace Fastboi;
using namespace Fastboi::Components;

namespace Adventure {
    struct Player final {
        inline static const float speed = 760.f;

        private:
        Rigidbody* rigidbody = nullptr;
        Input::KeyListener spacebarListener; 

        Position currentRoomPos = Vecf::zero();

        bool isEaten = false;

        public:
        GORef gameobject;
        Gameobject* hand = nullptr;

        Player(GORef&& go);
        ~Player();

        void Spacebar(const KeyEvent& e);
        void Start();
        void Update();

        void Eat();

        // void Collision(const CollisionEvent& e) const;

        static void Inst(Gameobject& go, const Position& pos);
    };

};