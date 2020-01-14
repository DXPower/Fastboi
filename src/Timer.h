#pragma once

#include "SDL/SDL.h"

struct Timer
{
    Uint64 previous_ticks{0};
    float elapsed_seconds{0};

    void tick()
    {
        const Uint64 current_ticks{ SDL_GetPerformanceCounter() };
        const Uint64 delta{ current_ticks - previous_ticks };
        previous_ticks = current_ticks;
        static const Uint64 TICKS_PER_SECOND{ SDL_GetPerformanceFrequency() };
        elapsed_seconds = delta / static_cast<float>(TICKS_PER_SECOND);
    }
};