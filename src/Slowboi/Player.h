#pragma once

#include "Fastboi.h"
#include "Spritesheet.h"

using namespace Fastboi;
using namespace Fastboi::Components;

namespace Slowboi {
    namespace Components {
        struct Player final {
            private:
            VelocityComp* velocityComp = nullptr;
            Spritesheet<int>* spritesheet = nullptr;

            Input::KeyListener spacebarListener;
            Input::ClickListener clickListener;

            Input::KeyListener enableListener;
            
            Vecf lastDirection;
            Vecf facingDirection;

            public:
            Gameobject& gameobject;
            float_t speed = 300.0f;

            Player(Gameobject& go);
            ~Player();

            void EnablePressed(const KeyEvent& e) const;
            void Spacebar(const KeyEvent& e) const;
            void VelocityChanged(const Velocity& velocity) const;
            void Fire(const ClickEvent& event) const;
            void Start();
            void Update();
            void Collision(const CollisionEvent& e) const;
        };
    };
};