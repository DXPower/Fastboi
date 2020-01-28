#include "Collision.h"
#include <algorithm>
#include "Collider.h"
#include "FastboiCore.h"
#include <functional>
#include "Gameobject.h"
#include <limits>
#include <stdint.h>
#include "VelocityComp.h"

using namespace Fastboi;
using namespace Fastboi::Collision;

extern "C" {
    #include "gjk.h"
}

gjk_vec2 ToGJKV(const Position& p) {
    return (gjk_vec2) { p.x, p.y };
}

Position ToPosition(const gjk_vec2& v) {
    return Position(v.x, v.y);
}


using Simplex = std::list<Position>;

namespace Fastboi {
    namespace Collision {
    // EPA - Expanding Polytype Algorithm
        namespace EPA {
            struct EdgeData {
                float distance;
                Vecf normal;
                Simplex::iterator index;

                EdgeData(float dist, Vecf norm, const Simplex::iterator& index) : distance(dist), normal(norm), index(index) { };
            };

            constexpr float TOLERANCE = 0.005f;

            EdgeData FindClosestEdge(Simplex& s) {
                EdgeData closest(0.f, Vecf::zero(), s.begin());
                closest.distance = std::numeric_limits<decltype(closest.distance)>::max();

                for (auto ait = s.begin(); ait != s.end(); ait++) {
                    auto bit = std::next(ait, 1);
                    if (bit == s.end()) bit = s.begin();

                    // Get next two points
                    const Position a = *ait; // Note: Position of A is also equal to vector from origin to A (OA)
                    const Position b = *bit;
                    Vecf edge = b - a;

                    Vecf normal = Vecf::tripleProduct(edge, a, edge).normalized(); // Get vector from edge towards origin
                    double dist = abs(Vecf::dotProduct(normal, a)); // Get distance from origin to edge

                    if (normal.magnitude2() >= TOLERANCE) {
                        if (dist < closest.distance) {
                            closest = EdgeData(dist, normal, bit);
                        }
                    }
                }

                return closest;
            }

            Vecf GetPenetration(Simplex& simplex, gjk_vec2* vertsA, size_t countA, gjk_vec2* vertsB, size_t countB) {
                Vecf penetration;

                // This prevents an extremely rare case where this iterates infinitely
                constexpr uint_fast8_t maxIter = 255;  
                uint_fast8_t iter = 0;
                while (iter != maxIter) {
                    // Find new support point in direction of the normal of the closest edge
                    EdgeData closestEdge = FindClosestEdge(simplex);
                    Position supportPoint = ToPosition(support(vertsA, countA, vertsB, countB, ToGJKV(closestEdge.normal)));
                    // Check if distance from origin is along e.normal
                    double d = Vecf::dotProduct(supportPoint, closestEdge.normal);

                    if (abs(d - closestEdge.distance) <= TOLERANCE) {
                        penetration = closestEdge.normal * d;
                        break;
                    } else {
                        simplex.insert(closestEdge.index, supportPoint); // Split the edge into two with the support point in the middle
                    }
                }

                return penetration;
            }
        }
    };
};

Collision_t::CollisionData Fastboi::Collision::AreShapesIntersecting(
      const circular_vector<Position>& shapeA
    , const circular_vector<Position>& shapeB
) {
    gjk_vec2* verticesA = new gjk_vec2[shapeA.size()];
    gjk_vec2* verticesB = new gjk_vec2[shapeB.size()];

    for (circular_vector<Position>::size_type i = 0; i < shapeA.size(); i++) {
        verticesA[i] = ToGJKV(shapeA[i]);
    } 
    
    for (circular_vector<Position>::size_type i = 0; i < shapeB.size(); i++) {
        verticesB[i] = ToGJKV(shapeB[i]);
    }

    gjk_vec2 terminationSimplex[3];
    bool intersecting = gjk(verticesA, shapeA.size(), verticesB, shapeB.size(), terminationSimplex);

    std::list<Position> simplex;

    for (Simplex::size_type i = 0; i < sizeof(terminationSimplex) / sizeof(terminationSimplex[0]); i++) {
        simplex.emplace_back(ToPosition(terminationSimplex[i]));
    }

    Vecf penetration = !intersecting ? Vecf::zero()
                                     : EPA::GetPenetration(simplex, verticesA, shapeA.size(), verticesB, shapeB.size());

    delete[] verticesA;
    delete[] verticesB;
    
    return { intersecting, penetration };
}

Collision_t::CollisionData Fastboi::Collision::AreCollidersIntersectng(
      const Collider& colliderA
    , const Collider& colliderB 
) {
    return AreShapesIntersecting(colliderA.GetVertices(), colliderB.GetVertices());
}

void Fastboi::Collision::ApplyVelocities(const std::vector<std::unique_ptr<Gameobject>>& gameobjects) {
    for (const std::unique_ptr<Gameobject>& go : gameobjects) {
        if (go->HasComponent<Components::VelocityComponent>()) {
            AdvanceTransform(*go->transform, go->GetComponent<Components::VelocityComponent>().velocity * Fastboi::physicsDelta);
        }
    }
}

void Fastboi::Collision::BroadPhase(
      const Colliders_t& colliders
    , PotentialCollisions_t& potentialCollisions
) {
    for (Collider* collider : colliders) {
        if (collider->isStarted && !collider->isDeleted) {
            potentialCollisions.push_back(collider);
        }
    }
}

void Fastboi::Collision::AdvanceTransform(Transform& transform, const Velocity& velocity) {
    transform.position += velocity;
}

void Fastboi::Collision::NarrowPhase(
      const PotentialCollisions_t& potentialCollisions
    , Collisions_t& collisions) {

    for (auto ca = potentialCollisions.begin(); ca != potentialCollisions.end(); ca++) {
        for (auto cb = std::next(ca, 1); cb != potentialCollisions.end(); cb++) {
            if (*ca == *cb) continue;
            if (Collision_t::CollisionData coll = AreCollidersIntersectng(**ca, **cb); coll.collided) {
               collisions.AddCollision({ **ca, **cb, coll });
            }
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

        AdvanceTransform(*collision.a.gameobject.transform, get<0>(finalVelocities));
        AdvanceTransform(*collision.b.gameobject.transform, get<1>(finalVelocities));
    }
}

//TODO: Profile performance between separate DispatchCollisions() and merging it into ResolveColliders
void Fastboi::Collision::DispatchCollisions(const Collisions_t& collisions) {
    for (const Collision_t& collision : collisions.Get()) {
        if (collision.a.isDeleted || collision.b.isDeleted) continue;
        
        collision.a.Collide(collision.b);
        collision.b.Collide(collision.a);
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
    // cCollisions.emplace(collision.a, data);
    // cCollisions.emplace(collision.b, data);
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

