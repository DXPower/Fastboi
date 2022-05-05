#include "Collision.h"
#include <algorithm>
#include "Collider.h"
#include "FastboiCore.h"
#include <functional>
#include "Gameobject.h"
#include "GameobjectAllocator.h"
#include <limits>
#include "Rigidbody.h"
#include <stdint.h>

#define CUTE_C2_IMPLEMENTATION
#include "cute_c2.h"

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

using namespace Fastboi;
using namespace Fastboi::Collision;

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
    for (auto it = colliders.begin(); it != colliders.end(); it++) {
        Collider* collider = *it;

        // if (!collider->isStarted || !collider->isEnabled || collider->isDeleted) continue;
        if (!collider->isStarted) continue;
        if (!collider->isEnabled) continue;
        if (collider->isDeleted) continue;

        // auto pots = aabbTree.query(collider);
        const Transform& ct = *collider->gameobject().transform;

        for (auto pcit = std::next(it); pcit != colliders.end(); pcit++) {
            Collider* pc = *pcit;

            if (!CollisionMask::CanCollide(collider->mask, pc->mask)) continue;

            if (unlikely(!pc->isStarted)) continue;
            if (unlikely(!pc->isEnabled)) continue;
            if (unlikely(pc->isDeleted)) continue;

            const Transform& pct = *pc->gameobject().transform;

            if (!BoundingBox::Overlaps(ct.GetBounds().Fatten(0.2f), pct.GetBounds().Fatten(0.2f))) continue;

            // Rendering::Request_Render_DebugRect(b.Fatten(0.2f).ToRect());
            // Rendering::Request_Render_DebugRect(ct.GetBounds().Fatten(0.2f).ToRect());

            potentialCollisions.emplace(ColliderPairKey(collider, pc));
        }
    }
}

void Fastboi::Collision::AdvanceTransform(Transform& transform, const Velocity& vel, const Degree& rotV) {
    transform.position += vel;
    transform.rotation += rotV;
}

void Fastboi::Collision::NarrowPhase(
      const PotentialCollisions_t& potentialCollisions
    , Collisions_t& collisions) {

    for (const auto& potCol : potentialCollisions) {
        Collider* const ca = potCol.a;
        Collider* const cb = potCol.b;

        // printf("Possible collision between %s and %s\n", ca->gameobject().name.c_str(), cb->gameobject().name.c_str());

        if (Transform::IsDescendentRelated(*ca->gameobject().transform, *cb->gameobject().transform)) continue;

        if (Collision_t::CollisionData coll = AreCollidersIntersectng(*ca, *cb); coll.collided) {
            collisions.AddCollision({ .a = *ca, .b = *cb, .data = coll });
        }
    }
}

using std::tuple;
using std::get;

void Fastboi::Collision::ResolveColliders(const Collisions_t& collisions) {
    for (const Collision_t& collision : collisions.Get()) {
        const tuple<Velocity, Velocity> finalVelocities = ResolveCollision(collision);

        AdvanceTransform(*collision.a.gameobject().transform, get<0>(finalVelocities), 0_deg);
        AdvanceTransform(*collision.b.gameobject().transform, get<1>(finalVelocities), 0_deg);

        if (get<0>(finalVelocities) != Velocity::zero() && collision.a.gameobject().HasComponent<Rigidbody>()) {
            Velocity& curVel = collision.a.gameobject().GetComponent<Rigidbody>().velocity;
            curVel = Velocity::projection(curVel, get<0>(finalVelocities).ortho());
        }

        if (get<1>(finalVelocities) != Velocity::zero() && collision.b.gameobject().HasComponent<Rigidbody>()) {
            Velocity& curVel = collision.b.gameobject().GetComponent<Rigidbody>().velocity;
            curVel = Velocity::projection(curVel, get<1>(finalVelocities).ortho());
        }
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

