#include "CollisionMask.h"

using namespace Fastboi;

bool CollisionMask::CanCollide(const CollisionMask& a, const CollisionMask& b) {
    return a.CanCollide(b.layer) && b.CanCollide(a.layer);
}

bool CollisionMask::CanCollide(CollisionLayer layer) const {
    using UT = std::underlying_type_t<CollisionLayer>;
    
    if (inclusions == CollisionLayer::NONE) return false;
    
    return inclusions == CollisionLayer::ALL ||
            (static_cast<UT>(inclusions) & static_cast<UT>(layer)) == static_cast<UT>(layer);
}