#include "Fastboi.h"

using namespace Fastboi;

namespace Minecraft {
    struct Player {
        GORef gameobject;

        Input::KeyListener keyListener;

        Player(GORef&& go) : gameobject(std::move(go)), keyListener(SDL_SCANCODE_W) {
            keyListener.signal.connect<&Player::KeyPress>(*this);
        }

        void KeyPress(const KeyEvent& e) const;
        void Update();

        static void Inst(Gameobject& go, Position pos);
    };
}