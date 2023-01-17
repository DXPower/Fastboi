#include "Player.h"
#include "Archetype.h"
#include "Events.h"
#include "FastboiComps.h"
#include "Input.h"
#include "Level.h"
#include "Room.h"
#include "Speaker.h"

using namespace Fastboi;
using namespace Input;
using namespace Adventure;

Player::Player(GORef&& go) : gameobject(std::move(go)) {
    Level::SetPlayer(go());
    Level::roomChangeSignal.connect<&Player::RoomChanged>(this);
}

Player::~Player() {
    gameobject().RemoveComponent<Rigidbody>();
}


void Player::Start() {
    gameobject().AddComponent<Speaker>().SetPan<PanType::POSITIONAL>();
}

void Player::System(Gameobject& go [[maybe_unused]], Player& player, Collider& collider, Rigidbody& rigidbody) {
    if (player.isEaten) return;

    Vecf direction(0, 0);

    if (Input::IsKeyDown(SDL_SCANCODE_W, SDL_SCANCODE_UP)) {
        direction.y -= 1;
    }

    if (Input::IsKeyDown(SDL_SCANCODE_S, SDL_SCANCODE_DOWN)) {
        direction.y += 1;
    }

    if (Input::IsKeyDown(SDL_SCANCODE_D, SDL_SCANCODE_RIGHT)) {
        direction.x += 1;
    }

    if (Input::IsKeyDown(SDL_SCANCODE_A, SDL_SCANCODE_LEFT)) {
        direction.x -= 1;
    }

    if (player.isInBridge) {
        player.isPhasing = false;

        for (const Collider* col : collider.GetCurrentCollisions()) {
            if (col->IsFixed()) {
                direction.x = 0; // Can't move sideways in bridge
                player.isPhasing = true;
                break;
            }
        }
    }

    rigidbody.velocity = direction.normalized() * speed;
}

void Player::Eat() {
    isEaten = true;
    gameobject().GetComponent<Rigidbody>().velocity = Velocity::zero();

    gameobject().collider->SetEnabled(false);
    gameobject().GetComponent<Speaker>().PlaySound(Resources::GetSound("Death"));
}

void Player::RoomChanged(const RoomChangeEvent& e) {
    printf("Changed room!\n");
    
    // Fastboi::camera.SetTarget(*new Transform(e.room.GetCenter()), Camera::OWNING);
    gameobject().GetComponent<ColorComp>() = e.room.GetColor();
}

void Player::EnterBridge() {
    printf("Enter bridge!\n");
    gameobject().collider->SetFlags(Collider::TRIGGER);
    isInBridge = true;
}

void Player::ExitBridge() {
    printf("Exit bridge!\n");
    gameobject().collider->SetFlags(Collider::NONE);
    isInBridge = false;
    isPhasing = false;
}

void Player::Inst(Gameobject& go, const Position& pos) {
    go.name = "Player";

    Fastboi::RegisterSystem<&Player::System>();

    go.AddComponent<Transform>(pos, Room::GetTileSize() / 2.f, 0_deg);
    go.AddComponent<BoxColorRenderer>(RenderData(RenderOrder::UNITS, 10));
    go.AddComponent<ColorComp>(255, 0, 255, 255);
    go.AddComponent<Collider>(0, CollisionLayer::PLAYER);
    go.AddComponent<Rigidbody>();
    go.AddComponent<Player>();
}
