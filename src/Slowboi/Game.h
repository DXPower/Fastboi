#pragma once
#include "Fastboi.h"

struct ColorComp;
struct Player;

namespace Slowboi {
    void InitGame();


    struct Bullet final : public Gameobject {
        Bullet(const Position& p, const Velocity& v);
        ~Bullet() { };
        
        void Hit(const Fastboi::CollisionEvent& e);
    };

    struct PlayerActor final : public Gameobject {
        PlayerActor(const Position& p);
        ~PlayerActor() { };
    };

    struct Brick final : public Gameobject {
        Fastboi::Input::KeyListener expandListener;

        Brick(const Position& p);
        ~Brick();

        void Expand(const Fastboi::KeyEvent& e);
    };
    
    struct UISquare final : public Gameobject {
        UISquare(const Position& p, const Size& s, const ColorComp& color, int zindex);
        ~UISquare() { };
    };
}