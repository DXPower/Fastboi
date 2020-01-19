#pragma once

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

        RenderData(RenderOrder o) : RenderData(o, 0) { };
        RenderData(RenderOrder o, int z) : order(o), zindex(z) { };
    };  
};