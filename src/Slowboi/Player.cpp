#include "Player.h"
#include "Spritesheet.h"
#include "Gameobject.h"
#include "Game.h"
#include "Input.h"
#include "SDL/SDL.h"

using namespace Fastboi;
using namespace Input;
using namespace Slowboi::Components;

Player::Player(Gameobject& go)
 : gameobject(go)
 , spacebarListener(KeyListener(SDL_SCANCODE_SPACE))
 , enableGO(KeyListener(SDL_SCANCODE_E))
 , enableCollider(KeyListener(SDL_SCANCODE_C))
 , lastDirection(Vecf(0, 1)) {
    printf("Connecting player...\n\n");
    spacebarListener.signal.connect<&Player::Spacebar>(this);
    clickListener.signal.connect<&Player::Fire>(this);

    enableGO.signal.connect<&Player::EnablePressed>(this);
    enableCollider.signal.connect<&Player::EnablePressed>(this);
}

Player::~Player() {
    gameobject.RemoveComponent<Spritesheet<int>>();
    gameobject.RemoveComponent<VelocityComp>();
}

void Player::EnablePressed(const KeyEvent& e) const {
    Fastboi::Print("Enable pressed!\n");

    if (e.type == KeyEvent::DOWN) {
        if (e.key == SDL_SCANCODE_E)
            gameobject.SetEnabled(!gameobject.IsEnabled());
        else if (e.key == SDL_SCANCODE_C)
            gameobject.SetComponentEnabled<Collider>(!gameobject.IsComponentEnabled<Collider>());
    }
}

void Player::Spacebar(const KeyEvent& e) const {
    Instantiate<Slowboi::Bullet>(gameobject.transform->position, facingDirection.normalized() * speed);
}

void Player::Fire(const ClickEvent& event) const {
    if (event.type == ClickEvent::DOWN) {
        // Fastboi::Instantiate<UISquare>(event.pos, Size(25, 25), ColorComp(100, 0, 100, 255), 50);

        Vecf d = (Fastboi::camera.ScreenToWorldPos(event.pos) - gameobject.transform->position).normalized();

        Gameobject& bullet = Instantiate<Slowboi::Bullet>(gameobject.transform->position, d * speed);
        Fastboi::camera.SetTarget(*gameobject.transform, Camera::WATCHING);
    } else if (event.type == ClickEvent::UP) {
        // printf("Fire up! %i %i\n", event.pos.x, event.pos.y);
    }
}

void Player::Start() {
    spritesheet = &gameobject.GetComponent<Spritesheet<int>>();
    velocityComp = &gameobject.AddComponent<VelocityComp>();
}

void Player::Update() {
    Vecf direction(0, 0);

    if (Input::IsKeyDown(SDL_SCANCODE_W)) {
        direction.y -= 1;
    }

    if (Input::IsKeyDown(SDL_SCANCODE_S)) {
        direction.y += 1;
    }

    if (Input::IsKeyDown(SDL_SCANCODE_D)) {
        direction.x += 1;
    }

    if (Input::IsKeyDown(SDL_SCANCODE_A)) {
        direction.x -= 1;
    }

    if (lastDirection != direction) {
        int dirKey = 0;

        if (direction != Vecf::zero()) {
            if (direction.x == 0 && direction.y == 1) dirKey = 1;
            else if (direction.x == 1 && direction.y == 1) dirKey = 2;
            else if (direction.x == 1 && direction.y == 0) dirKey = 3;
            else if (direction.x == 1 && direction.y == -1) dirKey = 4;
            else if (direction.x == 0 && direction.y == -1) dirKey = 5;
            else if (direction.x == -1 && direction.y == -1) dirKey = 6;
            else if (direction.x == -1 && direction.y == 0) dirKey = 7;
            else if (direction.x == -1 && direction.y == 1) dirKey = 8;

            spritesheet->SetCurrentAnimation(dirKey);
        } else {
            if (lastDirection.x == 0 && lastDirection.y == 1) dirKey = 1;
            else if (lastDirection.x == 1 && lastDirection.y == 1) dirKey = 2;
            else if (lastDirection.x == 1 && lastDirection.y == 0) dirKey = 3;
            else if (lastDirection.x == 1 && lastDirection.y == -1) dirKey = 4;
            else if (lastDirection.x == 0 && lastDirection.y == -1) dirKey = 5;
            else if (lastDirection.x == -1 && lastDirection.y == -1) dirKey = 6;
            else if (lastDirection.x == -1 && lastDirection.y == 0) dirKey = 7;
            else if (lastDirection.x == -1 && lastDirection.y == 1) dirKey = 8; 

            spritesheet->SetCurrentAnimation(-dirKey);
        }
        
        lastDirection = direction;

        if (direction != Vecf::zero()) {
            facingDirection = direction;
        }
    }

    velocityComp->velocity = direction.normalized() * speed;

    if (Input::IsKeyDown(SDL_SCANCODE_UP))
        Fastboi::camera.zoom += 0.01f;
    else if (Input::IsKeyDown(SDL_SCANCODE_DOWN))
        Fastboi::camera.zoom -= 0.01f;
}

void Player::Collision(const CollisionEvent& e) const {
    if (e.type == CollisionEvent::BEGIN) {
        printf("Player collided with: %s\n", e.collider.gameobject.name);
    } else if (e.type == CollisionEvent::END) {
        printf("Player ended collision with: %s\n", e.collider.gameobject.name);
    }
}