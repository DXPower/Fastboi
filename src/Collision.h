#pragma once

#include <list>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <memory>
#include <tuple>
#include "CollisionMask.h"
#include "Vec.h"
#include <vector>
#include "circular_vector.h"
#include <span>
#include "AABBTree.h"

namespace Fastboi {
    struct Degree;
    struct Collider;
    struct Gameobject;
    struct Transform;


           Gameobject* GetGameobjectAtPosition(Position pos, CollisionMask::UT acceptable);
    inline Gameobject* GetGameobjectAtPosition(Position pos, CollisionLayer acceptable) {
        return GetGameobjectAtPosition(pos, static_cast<CollisionMask::UT>(acceptable));
    }

    struct RaytraceResult {
        Gameobject* hit;
        Position origin;
        Position ending;
    };

    RaytraceResult Raytrace(Position origin, Vecf direction, CollisionMask::UT acceptable, float maxDistance);
    inline RaytraceResult Raytrace(Position origin, Vecf direction, CollisionLayer acceptable, float maxDistance) {
        return Raytrace(origin, direction, static_cast<CollisionMask::UT>(acceptable), maxDistance);
    }

    namespace Collision {
        extern AABBTree globalAABB;

        struct ColliderPairKey {
            Collider* a;
            Collider* b;

            ColliderPairKey(Collider* a, Collider* b);
            ColliderPairKey();
            bool operator==(const ColliderPairKey& rhs) const;

            struct Hash {
                std::size_t operator()(const ColliderPairKey& cp) const;
            };
        };

        struct Collision_t {
            struct CollisionData {
                bool collided;
                Vecf penetration;
            };

            Collider& a;
            Collider& b;
            CollisionData data;
        };

        using Vertices_t = circular_vector<Position>;
        using Colliders_t = std::unordered_set<Collider*>;
        using PotentialCollisions_t = std::unordered_set<ColliderPairKey, ColliderPairKey::Hash>;
        
        struct Collisions_t {
            std::vector<Collision_t> collisions;

            // bool HasCollided(Collider* c) const;
            void AddCollision(const Collision_t& collision);    
            const decltype(collisions)& Get() const;        
        };

        Collision_t::CollisionData AreCollidersIntersectng(
              const Collider& colliderA
            , const Collider& colliderB
        );
        
        Collision_t::CollisionData AreShapesIntersecting(
              const Vertices_t& shapeA
            , const Vertices_t& shapeB
        );

        void ProgressRigidbodies();

        void BroadPhase(const Colliders_t& colliders, PotentialCollisions_t& potentialCollisions);
        void NarrowPhase(const PotentialCollisions_t& potentialCollisions, Collisions_t& collisions);
        void ResolveColliders(const Collisions_t& collisions);
        void DispatchCollisions(const Collisions_t& collisions);
        std::tuple<Velocity, Velocity> ResolveCollision(const Collision_t& collision);

        void AdvanceTransform(Transform& transform, const Velocity& vel, const Degree& rotV);
        circular_vector<Position> AdvanceCollider(const Collider&);

        void UpdateAABBTree(Colliders_t& colliders);
    }
}