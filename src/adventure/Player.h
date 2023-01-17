#pragma once

#include "Events.h"
#include "Fastboi.h"
#include "Input.h"

using namespace Fastboi;
using namespace Fastboi::Components;

namespace Adventure {
    struct RoomChangeEvent;

    struct Player final {
        inline static const float speed = 760.f;

        private:
        bool isEaten = false;
        bool isInBridge = false;

        public:
        GORef gameobject;
        bool isPhasing = false;

        Player(GORef&& go);
        ~Player();

        void Start();
        static void System(Gameobject& go, Player& player, Collider& col, Rigidbody& rb);

        void Eat();
        void RoomChanged(const RoomChangeEvent& e);

        void EnterBridge();
        void ExitBridge();
        void Collision(const CollisionEvent& e);

        static void Inst(Gameobject& go, const Position& pos);
    };

};