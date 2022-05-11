#pragma once

#include <list>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <memory>
#include <tuple>
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
            // using CollisionPairMap_t = std::unordered_map<ColliderPairKey, CollisionData, ColliderPairKey::Hash>;
            std::vector<Collision_t> collisions;
            // Colliders_t cCollisions;

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
        namespace detail {
            void BroadPhaseHelper(std::span<const AABBTree::Node> nodes, PotentialCollisions_t& potentialCollisions, const AABBTree::Node& single, const AABBTree::Node& potential);
        }

        void NarrowPhase(const PotentialCollisions_t& potentialCollisions, Collisions_t& collisions);
        void ResolveColliders(const Collisions_t& collisions);
        void DispatchCollisions(const Collisions_t& collisions);
        std::tuple<Velocity, Velocity> ResolveCollision(const Collision_t& collision);

        void AdvanceTransform(Transform& transform, const Velocity& vel, const Degree& rotV);
        circular_vector<Position> AdvanceCollider(const Collider&);

        // AABBHandle RegisterAABB(const Collider& collider);
        void UpdateAABBTree(Colliders_t& colliders);
    }
}