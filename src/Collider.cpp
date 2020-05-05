#include "Collider.h"
#include <algorithm>
#include "Gameobject.h"
#include "Events.h"
#include "ChangeObserver.h"
#include "FastboiCore.h"
#include "BoundingBoxRenderer.h"

using namespace Fastboi;

Collider::Collider(GORef&& go) : Collider(std::move(go), 0, CollisionLayer::WALLS) { };

Collider::Collider(GORef&& gameobject, uint_fast8_t flags, CollisionLayer layer)
 : gameobject(std::move(gameobject))
 , flags(flags), mask(CollisionMask(layer)) {
    Fastboi::RegisterCollider(this);
};

Collider::Collider(const Collider& copy) : Collider(GORef(copy.gameobject), copy.flags, CollisionLayer::WALLS) {
    isEnabled = copy.isEnabled;
    isStarted = false;
    mask = CollisionMask(copy.mask);
}


Collider::~Collider() {
    CleanHangingCollisions();
    Fastboi::UnregisterCollider(this);

    if (boundingBox != nullptr)
        Fastboi::Destroy(*boundingBox);

    Collision::aabbTree.removeParticle(this);
}

void Collider::Start() {
    isStarted = true;

    // const Transform& tr = gameobject().GetComponent<Transform>();
    // BoundingBox bounds = tr.GetBounds();

    // Collision::aabbTree.insertParticle(this, bounds.lowerBounds, bounds.upperBounds);
}

void Collider::Update() {
    if (!isEnabled) return;

    // if (gameobject().name == std::string("Player")) {
        // if (boundingBox == nullptr) {
        //     boundingBox = &Fastboi::Instantiate<Gameobject>(gameobject().name);
        //     boundingBox->AddComponent<Transform>(*gameobject().transform);
        //     boundingBox->AddComponent<BoundingBoxRenderer>(RenderData(RenderOrder::UI, 1));
        // } else {
        //     *boundingBox->transform = *gameobject().transform;
    // }
        // }
    // const BoundingBox bounds = gameobject().transform->GetBounds(); 
    // Collision::aabbTree.updateParticle(this, bounds.lowerBounds, bounds.upperBounds);

    decltype(pendingCollisions) newCollisions;
    decltype(pendingCollisions) endingCollisions;

    GetNewAndEndingCollisions(newCollisions, endingCollisions);

    #define contains(container, val) (std::find(container.begin(), container.end(), val) != container.end())

    for (const ColDat& c : newCollisions) {
        if (!contains(currentCollisions, c)) {
            // New collision, trigger begin event
            CollisionEvent e(*c.col, CollisionEvent::BEGIN, c.pen);
            collisionSignal.fire(e);
        }
    }

    #undef contains

    for (const ColDat& c : endingCollisions) {
        // Ending collision, trigger end event
        CollisionEvent e(*c.col, CollisionEvent::END, c.pen);
        collisionSignal.fire(e);
    }

    currentCollisions = pendingCollisions;
    pendingCollisions.clear();

}

std::vector<Collider*> Collider::GetCurrentCollisions() const {
    std::vector<Collider*> colls(currentCollisions.size(), nullptr);

    size_t i = 0;
    for (const auto& d : currentCollisions) {
        colls[i] = d.col;
        i++;
    }

    return colls;
}

const circular_vector<Position>& Collider::GetVertices() const {
    return gameobject().transform->GetVertices();
}

void Collider::Collide(Collider& collider, const Vecf& pen) {
    pendingCollisions.push_back({ .col=&collider, .pen=pen });
}

void Collider::GetNewAndEndingCollisions(decltype(pendingCollisions)& newCs, decltype(pendingCollisions)& endingCs) {
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
    #define curs c.col->currentCollisions 
    #define pens c.col->pendingCollisions 

    for (const ColDat& c : currentCollisions) {
        curs.erase(std::remove(curs.begin(), curs.end(), this), curs.end());
        pens.erase(std::remove(pens.begin(), pens.end(), this), pens.end());
    }

    for (const ColDat& c : pendingCollisions) {
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