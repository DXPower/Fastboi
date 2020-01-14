#pragma once

#include "circular_vector.h"
#include "Vec.h"
#include "Collision.h"
#include "Events.h"
#include <list>
#include <unordered_set>
#include <vector>
#include <memory>

struct Gameobject;
struct Transform;
struct VelocityComponent;

struct Collider {
    private:
    /**
     * Because physics is on a separate thread from updates, a collision is only allowed to trigger any updates during an
     * update tick. We must keep track of what our current collisions are, as well as and collisions that happened
     * outside the update tick.
    **/
    std::vector<const Collider*> currentCollisions;
    std::vector<const Collider*> pendingCollisions;

    bool isStarted = false;
    bool isDeleted = false;
    
    public:
    Fastboi::Signal<void(const Fastboi::CollisionEvent&)> collisionSignal;
    Gameobject* gameobject;
    bool isTrigger, isFixed;

    Collider(Gameobject* gameobject);
    Collider(Gameobject* gameobject, bool isTrigger, bool isFixed);
    virtual ~Collider();
    
    void Start();
    void Update();
    void Destroy() { isDeleted = true; };

    const circular_vector<Position>& GetVertices() const;    

    private:
    void Collide(const Collider& collider);
    void GetNewAndEndingCollisions(std::vector<const Collider*>& newCs, std::vector<const Collider*>& endingCs);

    using Colliders_t = Fastboi::Collision::Colliders_t;
    using PotentialCollisions_t = Fastboi::Collision::PotentialCollisions_t;
    using Collisions_t = Fastboi::Collision::Collisions_t;
    using Collision_t = Fastboi::Collision::Collision_t;

    friend void Fastboi::Collision::BroadPhase(const Colliders_t&, PotentialCollisions_t&);
    friend void Fastboi::Collision::NarrowPhase(const PotentialCollisions_t&, Collisions_t&);
    friend void Fastboi::Collision::ResolveColliders(const Colliders_t&, const Collisions_t&);
    friend std::tuple<Velocity, Velocity> Fastboi::Collision::ResolveCollision(const Collision_t&);
    friend void Fastboi::Collision::DispatchCollisions(const Collisions_t&);
};