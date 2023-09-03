#include "Rigidbody.h"
#include "FastboiCore.h"
#include "Transform.h"

using namespace Fastboi;

template<typename T>
T clamp(T low, T val, T high) {
    return std::max(low, std::min(val, high));
}

void Rigidbody::ApplyParameters(Transform& t) {
    t.position += velocity * Fastboi::physicsDelta;
    t.rotation += rotVelocity * Fastboi::physicsDelta;

    velocity += acceleration * Fastboi::physicsDelta;
    
    velocity.x = clamp(-maxVelocity.x, velocity.x, maxVelocity.x);
    velocity.y = clamp(-maxVelocity.y, velocity.y, maxVelocity.y);

    rotVelocity += rotAcceleration * Fastboi::physicsDelta;

    const Vec<float> scaledDrag = drag * Fastboi::physicsDelta;

    // Absolute value of the new velocity
    const Velocity newAbsVel(std::max(0.f, std::abs(velocity.x) - scaledDrag.x), std::max(0.f, std::abs(velocity.y) - scaledDrag.y));
    // Create a vector with magnitudes of newAbsVel but signs of velocity
    velocity = Velocity(std::copysign(newAbsVel.x, velocity.x), std::copysign(newAbsVel.y, velocity.y));

    const double newAbsRV = std::max(0., std::abs(rotVelocity.Value()) - rotDrag.Value() * Fastboi::physicsDelta);
    rotVelocity = Degree(std::copysign(newAbsRV, rotVelocity.Value()));
}