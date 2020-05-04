#include "Player.h"
#include "Input.h"
#include "FastboiComps.h"
#include "Room.h"

using namespace Fastboi;
using namespace Input;
using namespace Adventure;

Player::Player(GORef&& go)
 : gameobject(std::move(go))
 , spacebarListener(KeyListener(SDL_SCANCODE_SPACE)) {
    spacebarListener.signal.connect<&Player::Spacebar>(this);
}

Player::~Player() {
    gameobject().RemoveComponent<Rigidbody>();

    if (hand != nullptr)
        delete hand;
}

void Player::Start() {
    // gameobject().GetComponent<Collider>().collisionSignal.connect<&Player::Collision>(this);
    rigidbody = &gameobject().AddComponent<Rigidbody>();

    currentRoomPos = Room::GetRoomCenterFromWorldPos(gameobject().transform->position);
    Fastboi::camera.SetTarget(*new Transform(currentRoomPos), Camera::OWNING);
    // Fastboi::camera.SetTarget(*gameobject().transform, Camera::WATCHING);
}

void Player::Spacebar(const KeyEvent& e) {
    // Instantiate<Slowboi::Bullet>(gameobject().transform->position, facingDirection.normalized() * speed, *this);
}

void Player::Update() {
    if (Position newRoomPos = Room::GetRoomCenterFromWorldPos(gameobject().transform->position); (Vec<int>) newRoomPos != (Vec<int>) currentRoomPos) {
        printf("New room pos: %f %f\n", newRoomPos.x, newRoomPos.y);
        currentRoomPos = newRoomPos;
        Fastboi::camera.SetTarget(*new Transform(currentRoomPos), Camera::OWNING);
    }

    Vecf direction(0, 0);

    if (Input::IsKeyDown(SDL_SCANCODE_SPACE)) {
        Spacebar(KeyEvent(SDL_SCANCODE_SPACE, KeyEvent::DOWN));
    }

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

    rigidbody->velocity = direction.normalized() * speed;
}

void Adventure::PlayerGO(Gameobject& go, const Position& pos) {
    go.name = "Player";

    go.AddComponent<Transform>(pos, Room::GetTileSize() / 2.f, 0);
    go.AddComponent<BoxColorRenderer>(RenderData(RenderOrder::UNITS, 10));
    go.AddComponent<ColorComp>(255, 0, 255, 255);
    go.AddComponent<Collider>();
    go.AddComponent<Player>();
}

// void Player::Collision(const CollisionEvent& e) const {
//     // if (e.type == CollisionEvent::BEGIN) {
//     // } else if (e.type == CollisionEvent::END) {
//     // }
// }