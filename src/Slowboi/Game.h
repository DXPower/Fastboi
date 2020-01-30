#pragma once
#include "Fastboi.h"
#include "FastboiComps.h"

using namespace Fastboi;
using namespace Fastboi::Components;

namespace Slowboi {
    namespace Components {
        struct Player;
    };

    void InitGame();

    struct Bullet final : public Gameobject {
        Bullet(const Position& p, const Velocity& v);
        ~Bullet() = default;
        
        void Hit(const Fastboi::CollisionEvent& e);
    };

    struct PlayerActor final : public Gameobject {
        PlayerActor(const Position& p);
        ~PlayerActor() = default;
    };

    struct Brick final : public Gameobject {
        Fastboi::Input::KeyListener expandListener;
        
        Brick(const Position& p);
        ~Brick() = default;

        void Expand(const Fastboi::KeyEvent& e);
    };
    
    struct UISquare final : public Gameobject {
        UISquare(const Position& p, const Size& s, const ColorComp& color, int zindex);
        ~UISquare() { };
    };
}