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

    void Bullet(Gameobject& go, const Position& p, const Velocity& v);
    void Brick(Gameobject& go, const Position& p);
    void UISquare(Gameobject& go, const Position& p, const Size& s, const ColorComp& color, int zindex);
    void PlayerGO(Gameobject& go, const Position& p);
    // struct Brick final : public Gameobject {
    //     Fastboi::Input::KeyListener expandListener;
        
    //     Brick(const Position& p);
    //     ~Brick() = default;

    //     void Expand(const Fastboi::KeyEvent& e);
    // };
    
    // struct UISquare final : public Gameobject {
    //     UISquare(const Position& p, const Size& s, const ColorComp& color, int zindex);
    //     ~UISquare() { };
    // };
}