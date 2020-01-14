#include "Collider.h"
#include <algorithm>
#include "Gameobject.h"
#include "Events.h"
#include "ChangeObserver.h"
#include "FastboiCore.h"
#include "VelocityComp.h"

using namespace Fastboi;

Collider::Collider(Gameobject* gameobject) : Collider(gameobject, false, false) { };

Collider::Collider(Gameobject* gameobject, bool isTrigger, bool isFixed)
 : gameobject(gameobject)
 , isTrigger(isTrigger)
 , isFixed(isFixed) {
    Fastboi::RegisterCollider(this);
};

Collider::~Collider() {
    Fastboi::UnregisterCollider(this);
}

void Collider::Start() {
    isStarted = true;
}

void Collider::Update() {
    std::vector<const Collider*> newCollisions;
    std::vector<const Collider*> endingCollisions;

    GetNewAndEndingCollisions(newCollisions, endingCollisions);

    #define contains(container, val) (std::find(container.begin(), container.end(), val) != container.end())

    for (const Collider* c : newCollisions) {
        if (!contains(currentCollisions, c)) {
            // New collision, trigger begin event
            CollisionEvent e(*c, CollisionEvent::BEGIN);
            collisionSignal.fire(e);
        }
    }

    #undef contains

    for (const Collider* c : endingCollisions) {
        // Ending collision, trigger end event
        CollisionEvent e(*c, CollisionEvent::END);
        collisionSignal.fire(e);
    }

    currentCollisions = pendingCollisions;
    pendingCollisions.clear();
}

const circular_vector<Position>& Collider::GetVertices() const {
    return gameobject->transform->GetVertices();
}

void Collider::Collide(const Collider& collider) {
    pendingCollisions.push_back(&collider);
}

void Collider::GetNewAndEndingCollisions(std::vector<const Collider*>& newCs, std::vector<const Collider*>& endingCs) {
    // Prerequisite for std::set_difference
    std::sort(currentCollisions.begin(), currentCollisions.end());
    std::sort(pendingCollisions.begin(), pendingCollisions.end());

    std::set_difference(pendingCollisions.begin(), pendingCollisions.end(),
                        currentCollisions.begin(), currentCollisions.end(),
                        std::back_inserter(newCs));

    std::set_difference(currentCollisions.begin(), currentCollisions.end(),
                        pendingCollisions.begin(), pendingCollisions.end(),
                        std::back_inserter(endingCs));
}
