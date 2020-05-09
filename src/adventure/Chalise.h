#pragma once

#include "Fastboi.h"

using namespace Fastboi;
using namespace Fastboi::Components;

namespace Fastboi::Components {
    struct ColorComp;
}

namespace Adventure {
    struct RoomChangeEvent;

    struct Chalise {
        private:
        bool playWinSequence = false;

        public:
        GORef go;
        ColorComp& c;

        Chalise(GORef&& go, ColorComp& color);

        void Update();
        void RoomChanged(const RoomChangeEvent& e);

        static void Inst(Gameobject& go, const Position& pos);
    };

}