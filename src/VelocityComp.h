#pragma once

#include <Vec.h>

namespace Fastboi {
    namespace Components {
        struct VelocityComponent {
            Velocity velocity;

            VelocityComponent(const Velocity& v) : velocity(v) { };
            VelocityComponent() : VelocityComponent(Velocity::zero()) { };
        };
    };
};