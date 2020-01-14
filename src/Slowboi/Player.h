#pragma once

#include "Fastboi.h"
#include "Spritesheet.h"

struct Player final {
    private:
    VelocityComponent* velocityComp;
    Spritesheet<int>* spritesheet;

    Fastboi::Input::KeyListener spacebarListener;
    Fastboi::Input::ClickListener clickListener;
    
    Vecf lastDirection;
    Vecf facingDirection;

    public:
    Gameobject* gameobject;
    float_t speed = 500.0f;

    Player(Gameobject* go);
    ~Player();

    void Spacebar(const Fastboi::KeyEvent& e) const;
    void VelocityChanged(const Velocity& velocity) const;
    void Fire(const Fastboi::ClickEvent& event) const;
    void Start();
    void Update();
    void Collision(const Fastboi::CollisionEvent& e) const;
};