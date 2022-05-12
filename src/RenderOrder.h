#pragma once

#include <compare>

namespace Fastboi {
    enum class RenderOrder {
        SKYBOX,
        GROUND,
        OBJECTS_UNDER,
        UNITS,
        PARTICLES,
        OBJECTS_OVER,
        UI
    };

    struct RenderData {
        RenderOrder order;
        int zindex;

        constexpr RenderData(RenderOrder o) : RenderData(o, 0) { };
        constexpr RenderData(RenderOrder o, int z) : order(o), zindex(z) { };

        constexpr bool operator==(const RenderData& other) const = default;

        constexpr std::strong_ordering operator<=>(const RenderData& rhs) const {
            if (order != rhs.order)
                return order <=> rhs.order;
            else
                return zindex <=> rhs.zindex;
        }
    };  
};