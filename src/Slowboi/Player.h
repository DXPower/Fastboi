#pragma once

#include "Fastboi.h"
#include "Spritesheet.h"

using namespace Fastboi;
using namespace Fastboi::Components;

namespace Slowboi {
    namespace Components {
        struct Player final {
            private:
            VelocityComponent* velocityComp;
            Spritesheet<int>* spritesheet;

            Input::KeyListener spacebarListener;
            Input::ClickListener clickListener;

            Input::KeyListener zoomIn, zoomOut;
            
            Vecf lastDirection;
            Vecf facingDirection;

            public:
            Gameobject* gameobject;
            float_t speed = 500.0f;

            Player(Gameobject* go);
            ~Player();

            void ChangeZoom(const KeyEvent& e) const;
            void Spacebar(const KeyEvent& e) const;
            void VelocityChanged(const Velocity& velocity) const;
            void Fire(const ClickEvent& event) const;
            void Start();
            void Update();
            void Collision(const CollisionEvent& e) const;
        };
    };
};