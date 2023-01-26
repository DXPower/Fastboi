#include "Player.h"
#include "CollisionMask.h"
#include "FastboiComps.h"
#include "Block.h"

#include <iostream>

using namespace Fastboi;
using namespace Minecraft;

void Player::Update() {

    Vec<int> dir{};
    constexpr float speed = 750.f;

    dir.x -= Input::IsKeyDown(SDL_SCANCODE_A);
    dir.x += Input::IsKeyDown(SDL_SCANCODE_D);

    auto& rb = gameobject().GetComponent<Rigidbody>();

    bool isTouchingGround = IsTouchingGround();

    if (isTouchingGround)
        gameobject().GetComponent<Components::ColorComp>().set(0, 0, 255, 255);
    else
        gameobject().GetComponent<Components::ColorComp>().set(150, 0, 250, 255);

    if (dir == Vec<int>::zero()) {
        if (isTouchingGround)
            rb.drag.x = 7500.f;
        else
            rb.drag.x = 1000.f;

        return;
    }
    
    gameobject().GetComponent<Rigidbody>().velocity.x = speed * dir.x;
}

bool Player::IsTouchingGround() const {
    const auto bounds = gameobject().transform->GetBounds();

    const Position  lowerLeft{bounds.lowerBounds.x, bounds.upperBounds.y + 1};
    const Position& lowerRight = bounds.upperBounds + Vecf{0, 1};

    for (const auto& p : { lowerLeft, lowerRight }) {
        auto res = Raytrace(p, {0, 1}, CollisionLayer::WALLS, 1.f);

        if (res.hit)
            return true;
    }

    return false;
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