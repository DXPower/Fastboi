#pragma once

#include "Fastboi.h"

using namespace Fastboi;
using namespace Fastboi::Components;

namespace Adventure {
    struct Player final {
        private:
        Rigidbody* rigidbody = nullptr;
        Input::KeyListener spacebarListener; 

        Position currentRoomPos = Vecf::zero();

        public:
        GORef gameobject;
        Gameobject* hand = nullptr;

        float_t speed = 760.f;

        Player(GORef&& go);
        ~Player();

        void Spacebar(const KeyEvent& e);
        void Start();
        void Update();
        // void Collision(const CollisionEvent& e) const;

        static void Inst(Gameobject& go, const Position& pos);
    };

};