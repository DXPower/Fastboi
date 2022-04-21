#include "CollisionMask.h"

using namespace Fastboi;

bool CollisionMask::CanCollide(const CollisionMask& a, const CollisionMask& b) {
    return a.CanCollide(b.layer) && b.CanCollide(a.layer);
}

bool CollisionMask::CanCollide(CollisionLayer layer) const {
    if (inclusions == static_cast<UT>(CollisionLayer::NONE)) return false;
    
    return inclusions == static_cast<UT>(CollisionLayer::ALL)
           || (inclusions & static_cast<UT>(layer)) == static_cast<UT>(layer);
}