#pragma once

#include "Fastboi.h"

using namespace Fastboi;
using namespace Fastboi::Components;

namespace Adventure {
    struct RoomChangeEvent;

    struct Player final {
        inline static const float speed = 760.f;

        private:
        Rigidbody* rigidbody = nullptr;

        bool isEaten = false;
        bool isInBridge = false;

        public:
        GORef gameobject;
        bool isPhasing = false;

        Player(GORef&& go);
        ~Player();

        void Start();
        void Update();

        void Eat();
        void RoomChanged(const RoomChangeEvent& e);

        void EnterBridge();
        void ExitBridge();
        void Collision(const CollisionEvent& e);

        static void Inst(Gameobject& go, const Position& pos);
    };

};