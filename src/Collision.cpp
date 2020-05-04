#include "Collision.h"
#include <algorithm>
#include "Collider.h"
#include "FastboiCore.h"
#include <functional>
#include "Gameobject.h"
#include "GameobjectAllocator.h"
#include <limits>
#include "Rendering.h"
#include "Rigidbody.h"
#include <stdint.h>

#define CUTE_C2_IMPLEMENTATION
#include "cute_c2.h"

using namespace Fastboi;
using namespace Fastboi::Collision;

AABBTree::Tree Collision::aabbTree(0.05, 128, true);

c2v toC2V(const Position& p) {
    return (c2v) { p.x, p.y };
}

Collision_t::CollisionData Fastboi::Collision::AreShapesIntersecting(
      const circular_vector<Position>& shapeA
    , const circular_vector<Position>& shapeB
) {
    if (shapeA.size() > C2_MAX_POLYGON_VERTS || shapeB.size() > C2_MAX_POLYGON_VERTS) {
        Application::ThrowRuntimeException("Too many vertices through GJK algorithm", Application::GJK_TOO_MANY_VERTICES);
    }

    c2Poly polyA = { (int) shapeA.size(), {}, {} }; 
    c2Poly polyB = { (int) shapeB.size(), {}, {} };

    for (circular_vector<Position>::size_type i = 0; i < shapeA.size(); i++) {
        polyA.verts[i] = toC2V(shapeA[i]);
    } 
    
    for (circular_vector<Position>::size_type i = 0; i < shapeB.size(); i++) {
        polyB.verts[i] = toC2V(shapeB[i]);
    } 

    c2MakePoly(&polyA);
    c2MakePoly(&polyB);

    c2Manifold manifold;
    c2PolytoPolyManifold(&polyA, nullptr, &polyB, nullptr, &manifold);

    if (manifold.count == 0)
        return { false, Vecf::zero() };
    else {
        float maxDepth = std::numeric_limits<float>::lowest();

        for (int i = 0; i < manifold.count; i++) {
            if (manifold.depths[i] > maxDepth)
                maxDepth = manifold.depths[i];
        }

        Vecf penetration = Vecf(manifold.n.x, manifold.n.y) * maxDepth;

        return { true, penetration };
    }
}

Collision_t::CollisionData Fastboi::Collision::AreCollidersIntersectng(
      const Collider& colliderA
    , const Collider& colliderB 
) {
    return AreShapesIntersecting(colliderA.GetVertices(), colliderB.GetVertices());
}

namespace Fastboi {
    extern GameobjectAllocator gameobjectAllocator;
}

void Fastboi::Collision::ProgressRigidbodies() {
    for (auto it = gameobjectAllocator.GO_Begin(); it != gameobjectAllocator.GO_End(); it++) {
        Gameobject& go = *it;

        if (go.isStarted && go.HasComponent<Rigidbody>() && go.IsComponentEnabled<Rigidbody>()) {
            go.GetComponent<Rigidbody>().ApplyParameters(*go.transform);

            // AdvanceTransform(*go.transform, rb.velocity * Fastboi::physicsDelta, rb.rotVelocity * Fastboi::physicsDelta);
        }
    }
}

void Fastboi::Collision::BroadPhase(
      const Colliders_t& colliders
    , PotentialCollisions_t& potentialCollisions
) {
    for (Collider* collider : colliders) {
        if (!collider->isStarted || !collider->isEnabled || collider->isDeleted) continue;

        auto pots = aabbTree.query(collider);

        for (Collider* pc : pots) {
            if (!pc->isStarted || !pc->isEnabled || pc->isDeleted) continue;

            const Transform& t = *pc->gameobject().transform;
            // Rendering::Request_Render_DebugRect(RectF(t.position.x, t.position.y, t.size.x, t.size.y));

            potentialCollisions.emplace(ColliderPairKey(collider, pc));
        }
    }
}

void Fastboi::Collision::AdvanceTransform(Transform& transform, const Velocity& vel, const double rotV) {
    transform.position += vel;
    transform.rotation += rotV;
}

void Fastboi::Collision::NarrowPhase(
      const PotentialCollisions_t& potentialCollisions
    , Collisions_t& collisions) {

    for (const auto& potCol : potentialCollisions) {
        Collider* const ca = potCol.a;
        Collider* const cb = potCol.b;

        if (ca == cb) printf("Attempting to compare against self!\n");
        if (Transform::IsDescendentRelated(*ca->gameobject().transform, *cb->gameobject().transform)) continue;

        if (Collision_t::CollisionData coll = AreCollidersIntersectng(*ca, *cb); coll.collided) {
            collisions.AddCollision({ .a = *ca, .b = *cb, .data = coll });
        }
    }
}

using std::tuple;
using std::get;

void Fastboi::Collision::ResolveColliders(
      const Colliders_t& allColliders
    , const Collisions_t& collisions
) {
    for (const Collision_t& collision : collisions.Get()) {
        tuple<Velocity, Velocity> finalVelocities = ResolveCollision(collision);

        AdvanceTransform(*collision.a.gameobject().transform, get<0>(finalVelocities), 0.);
        AdvanceTransform(*collision.b.gameobject().transform, get<1>(finalVelocities), 0.);
    }
}

//TODO: Profile performance between separate DispatchCollisions() and merging it into ResolveColliders
void Fastboi::Collision::DispatchCollisions(const Collisions_t& collisions) {
    for (const Collision_t& collision : collisions.Get()) {
        if (collision.a.isDeleted || collision.b.isDeleted) continue;
        
        collision.a.Collide(collision.b, -collision.data.penetration);
        collision.b.Collide(collision.a, collision.data.penetration);
    }
}

tuple<Velocity, Velocity> Fastboi::Collision::ResolveCollision(const Collision_t& collision) {
    tuple<Velocity, Velocity> finalVelocities;

    if (collision.a.IsTrigger() || collision.b.IsTrigger()) return finalVelocities; // Triggers do not affect velocities at all

    if (!collision.a.IsFixed()) {
        get<0>(finalVelocities) = -collision.data.penetration;
    } else
        get<0>(finalVelocities) = Velocity::zero();

    if (!collision.b.IsFixed())
        get<1>(finalVelocities) = collision.data.penetration;
    else
        get<1>(finalVelocities) = Velocity::zero();

    return finalVelocities;
}

void Collisions_t::AddCollision(const Collision_t& collision) {
    collisions.emplace_back(collision);
}

const std::vector<Collision_t>& Collisions_t::Get() const {
    return collisions;
}

ColliderPairKey::ColliderPairKey() : ColliderPairKey(nullptr, nullptr) { }
ColliderPairKey::ColliderPairKey(Collider* a, Collider* b) : a(a), b(b) { }

bool ColliderPairKey::operator==(const ColliderPairKey& rhs) const {
    return (a == rhs.a && b == rhs.b)
        || (a == rhs.b && b == rhs.a);
}

std::size_t ColliderPairKey::Hash::operator()(const ColliderPairKey& cp) const {
    std::size_t a = std::hash<Collider*>()(cp.a);
    std::size_t b = std::hash<Collider*>()(cp.b);

    // (a, b) and (b, a) always produce the same hash
    return std::min(a, b) ^ std::max(a, b);
}

