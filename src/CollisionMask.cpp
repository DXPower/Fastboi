#include "CollisionMask.h"

using namespace Fastboi;

bool CollisionMask::CanCollide(const CollisionMask& a, const CollisionMask& b) {
    return a.CanCollide(b.layer) && b.CanCollide(a.layer);
}

bool CollisionMask::CanCollide(const CollisionMask& a, UT b) {
    if ((b & static_cast<UT>(CollisionLayer::ALL)) == static_cast<UT>(CollisionLayer::ALL))
        return true;

    if ((b & static_cast<UT>(CollisionLayer::NONE)) == static_cast<UT>(CollisionLayer::NONE))
        return false;

    return (static_cast<UT>(a.layer) & b) != 0;
}

bool CollisionMask::CanCollide(CollisionLayer layer) const {
    if (inclusions == static_cast<UT>(CollisionLayer::NONE)) return false;
    
    return inclusions == static_cast<UT>(CollisionLayer::ALL)
        || (inclusions & static_cast<UT>(layer)) == static_cast<UT>(layer);
}