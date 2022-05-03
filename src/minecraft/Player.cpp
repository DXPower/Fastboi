#include "Player.h"
#include "FastboiComps.h"
#include "Block.h"

using namespace Fastboi;
using namespace Minecraft;

void Player::Update() {
    Vec<int> dir;
    constexpr float speed = 750.f;

    dir.x -= Input::IsKeyDown(SDL_SCANCODE_A);
    dir.x += Input::IsKeyDown(SDL_SCANCODE_D);

    if (dir == Vec<int>::zero())
        return;

    gameobject().GetComponent<Rigidbody>().velocity.x = speed * dir.x;
}

void Player::KeyPress(const KeyEvent& e) const {
    std::cout << "Key press!\n";
    gameobject().GetComponent<Rigidbody>().velocity.y = -850.f;
}

void Player::Inst(Gameobject& go, Position pos) {
    go.name = "Steve";

    go.AddComponent<Transform>(pos, Size(75, 200), 0_deg);
    go.AddComponent<Collider>(Collider::NONE, CollisionLayer::PLAYER);
    go.AddComponent<Rigidbody>().acceleration.y = 3000.f;

    go.AddComponent<Components::BoxColorRenderer>(RenderData(RenderOrder::OBJECTS_OVER));
    go.AddComponent<Components::ColorComp>(0, 0, 255, 255);
    go.AddComponent<Player>();

    std::cout << "Player instantiated\n";
}