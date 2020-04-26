#pragma once

#include <chrono>

#define ch std::chrono

namespace Fastboi {
    struct Timer
    {
        ch::steady_clock::time_point lastTick = ch::steady_clock::now();
        ch::duration<double, std::milli> elapsed_seconds{0};

        void Tick() {
            auto t2 = ch::steady_clock::now();
            elapsed_seconds = ch::duration<double, std::milli>(t2 - lastTick);
            lastTick = t2;
        }

        decltype(elapsed_seconds) TimeSinceLastTick() {
            auto t2 = ch::steady_clock::now();

            return ch::duration<double, std::milli>(t2 - lastTick);
        }
    };
};

#undef ch