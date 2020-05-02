#include "Collider.h"
#include <algorithm>
#include "Gameobject.h"
#include "Events.h"
#include "ChangeObserver.h"
#include "FastboiCore.h"

using namespace Fastboi;

Collider::Collider(GORef&& go) : Collider(std::move(go), 0) { };

Collider::Collider(GORef&& gameobject, uint_fast8_t flags)
 : gameobject(std::move(gameobject))
 , flags(flags) {
    Fastboi::RegisterCollider(this);
};

Collider::Collider(const Collider& copy) : Collider(GORef(copy.gameobject), copy.flags) {
    isEnabled = copy.isEnabled;
    isStarted = false;
}


Collider::~Collider() {
    CleanHangingCollisions();
    Fastboi::UnregisterCollider(this);
}

void Collider::Start() {
    isStarted = true;
}

void Collider::Update() {
    if (!isEnabled) return;

    std::vector<Collider*> newCollisions;
    std::vector<Collider*> endingCollisions;

    GetNewAndEndingCollisions(newCollisions, endingCollisions);

    #define contains(container, val) (std::find(container.begin(), container.end(), val) != container.end())

    for (Collider* c : newCollisions) {
        if (!contains(currentCollisions, c)) {
            // New collision, trigger begin event
            CollisionEvent e(*c, CollisionEvent::BEGIN);
            collisionSignal.fire(e);
        }
    }

    #undef contains

    for (Collider* c : endingCollisions) {
        // Ending collision, trigger end event
        CollisionEvent e(*c, CollisionEvent::END);
        collisionSignal.fire(e);
    }

    currentCollisions = pendingCollisions;
    pendingCollisions.clear();

}

const circular_vector<Position>& Collider::GetVertices() const {
    return gameobject().transform->GetVertices();
}

void Collider::Collide(Collider& collider) {
    pendingCollisions.push_back(&collider);
}

void Collider::GetNewAndEndingCollisions(std::vector<Collider*>& newCs, std::vector<Collider*>& endingCs) {
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

void Collider::CleanHangingCollisions() {
    #define curs c->currentCollisions 
    #define pens c->pendingCollisions 

    for (Collider* c : currentCollisions) {
        curs.erase(std::remove(curs.begin(), curs.end(), this), curs.end());
        pens.erase(std::remove(pens.begin(), pens.end(), this), pens.end());
    }

    for (Collider* c : pendingCollisions) {
        curs.erase(std::remove(curs.begin(), curs.end(), this), curs.end());
        pens.erase(std::remove(pens.begin(), pens.end(), this), pens.end());
    }

    #undef curs
    #undef pens
}

void Collider::SetEnabled(bool f) {
    if (!f) {
        pendingCollisions.clear();
        currentCollisions.clear();
    }

    isEnabled = f;
}