#include "Angles.h"

using namespace Fastboi;

// Degree::Degree(const Radian& rad) noexcept : value(FromRadians((double) rad)) { };

// Degree::operator Radian() const noexcept {
//     return Radian(ToRadians(value));    
// }

void TestComp() {
    constexpr Degree a = 30_deg;
    Degree b = 40_deg;

    b += a;
    b += 20_deg;

    b = 20_deg + 45_deg;
}