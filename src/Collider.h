#pragma once

#include "circular_vector.h"
#include "Vec.h"
#include "Collision.h"
#include "Events.h"
#include <GORef.h>
#include <list>
#include <unordered_set>
#include <vector>
#include <memory>


namespace Fastboi {
    struct Transform;

    struct Collider final {
        private:
        std::vector<Collider*> currentCollisions;
        std::vector<Collider*> pendingCollisions;

        bool isEnabled = true;
        bool isStarted = false;
        bool isDeleted = false;
        uint_fast8_t flags;
        
        public:
        enum {
            TRIGGER = 1 // Trigger means any collision will not affect either itself or other's positions, but still triggers event. (Like a tripwire)
            , FIXED = 2 // Fixed means any collision will not affect its own position, but still can affect another's position. (Like a wall)
        };

        Fastboi::Signal<void(const Fastboi::CollisionEvent&)> collisionSignal;
        GORef gameobject;

        Collider(GORef&& gameobject);
        Collider(GORef&& gameobject, uint_fast8_t flags);
        Collider(const Collider& copy);
        ~Collider();
        
        void Start();
        void Update();
        inline void Destroy() { isDeleted = true; };

        const circular_vector<Position>& GetVertices() const; 

        inline bool IsTrigger() const { return flags & TRIGGER; };
        inline bool IsFixed() const { return flags & FIXED; };  
        inline void SetFlags(uint_fast8_t flags) { this->flags = flags; };

        void SetEnabled(bool f);
        inline void Enable() { SetEnabled(true); };
        inline void Disable() { SetEnabled(false); };
        inline bool IsEnabled() const { return isEnabled; };

        private:
        void Collide(Collider& collider);
        void GetNewAndEndingCollisions(std::vector<Collider*>& newCs, std::vector<Collider*>& endingCs);
        void CleanHangingCollisions();

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
};