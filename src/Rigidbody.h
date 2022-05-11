#pragma once

#include "Angles.h"
#include "Vec.h"

namespace Fastboi {
    struct Transform;

    namespace Collision {
        void ProgressRigidbodies();
    }

    struct Rigidbody {
        Velocity velocity = Velocity::zero();
        Velocity maxVelocity = Velocity(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
        Velocity acceleration = Velocity::zero();
        float drag = 0.f;

        Degree rotVelocity = 0_deg;
        Degree rotAcceleration = 0_deg;
        Degree rotDrag = 0_deg;

        Rigidbody() = default;
        Rigidbody(Velocity v) : velocity(v) { }; 
        Rigidbody(Velocity v, Degree rv) : velocity(v), rotVelocity(rv) { };

        bool operator==(const Rigidbody& rb) const = default;

        private:
        void ApplyParameters(Transform& t);

        friend void Collision::ProgressRigidbodies();
    };
}