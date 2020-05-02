#include "Rigidbody.h"
#include "FastboiCore.h"
#include "Transform.h"

using namespace Fastboi;

void Rigidbody::ApplyParameters(Transform& t) {
    t.position += velocity * Fastboi::physicsDelta;
    t.SetRotation(t.rotation + rotVelocity * Fastboi::physicsDelta);

    velocity += acceleration * Fastboi::physicsDelta;
    rotVelocity += rotAcceleration * Fastboi::physicsDelta;

    const float scaledDrag = (float) drag * Fastboi::physicsDelta;

    // Absolute value of the new velocity
    const Velocity newAbsVel(std::max(0.f, std::abs(velocity.x) - scaledDrag), std::max(0.f, std::abs(velocity.y) - scaledDrag));
    // Create a vector with magnitudes of newAbsVel but signs of velocity
    velocity = Velocity(std::copysign(newAbsVel.x, velocity.x), std::copysign(newAbsVel.y, velocity.y));

    const double newAbsRV = std::max(0., std::abs(rotVelocity) - rotDrag * Fastboi::physicsDelta);
    rotVelocity = std::copysign(newAbsRV, rotVelocity);
}