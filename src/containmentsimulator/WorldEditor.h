#pragma once

#include "Fastboi.h"
#include "Button.h"

namespace CS {
    using namespace Fastboi;

    class WorldEditor {
        std::vector<Button*> groundTileButtons;

        public:
        WorldEditor();
    };
}