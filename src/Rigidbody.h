#pragma once

#include "Vec.h"

namespace Fastboi {
    struct Transform;

    namespace Collision {
        void ProgressRigidbodies();
    }

    struct Rigidbody {
        Velocity velocity = Velocity::zero();
        Velocity acceleration = Velocity::zero();
        float drag = 0.f;

        double rotVelocity = 0.;
        double rotAcceleration = 0.;
        double rotDrag = 0.;

        Rigidbody() = default;
        Rigidbody(Velocity v) : velocity(v) { }; 
        Rigidbody(Velocity v, double rv) : velocity(v), rotVelocity(rv) { };

        private:
        void ApplyParameters(Transform& t);

        friend void Collision::ProgressRigidbodies();
    };
}