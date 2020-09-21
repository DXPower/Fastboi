#pragma once

#include "Fastboi.h"
#include "Button.h"
#include "TileData.h"

namespace CS {
    using namespace Fastboi;

    class WorldEditor {
        std::vector<Button*> groundTileButtons;
        inline static TileID currentTileSel = TileID::NONE;

        public:
        GORef go;

        WorldEditor(GORef&& go);

        void ButtonClick(const ButtonEvent& e);
        static TileID GetSelectedTile() { return currentTileSel; };

        static void Blueprint(Gameobject& go, Position pos);
    };
}