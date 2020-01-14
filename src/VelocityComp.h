#pragma once

#include <Vec.h>

using Velocity = Vecf;

struct VelocityComponent {
    Velocity velocity;

    VelocityComponent(const Velocity& v) : velocity(v) { };
    VelocityComponent() : VelocityComponent(Velocity::zero()) { };
};