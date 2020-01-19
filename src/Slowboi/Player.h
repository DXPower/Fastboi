#pragma once

#include "Fastboi.h"
#include "Spritesheet.h"

using namespace Fastboi;
using namespace Components;

namespace Slowboi {
    namespace Components {
        struct Player final {
            private:
            VelocityComponent* velocityComp;
            Spritesheet<int>* spritesheet;

            Input::KeyListener spacebarListener;
            Input::ClickListener clickListener;
            
            Vecf lastDirection;
            Vecf facingDirection;

            public:
            Gameobject* gameobject;
            float_t speed = 500.0f;

            Player(Gameobject* go);
            ~Player();

            void Spacebar(const KeyEvent& e) const;
            void VelocityChanged(const Velocity& velocity) const;
            void Fire(const ClickEvent& event) const;
            void Start();
            void Update();
            void Collision(const CollisionEvent& e) const;
        };
    };
};