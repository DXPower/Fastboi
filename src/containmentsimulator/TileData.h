#pragma once

#include "Rect.h"

namespace CS {
    enum class TileID {
        WHITE_MARBLE = 0
        , GREY_SQUARES = 1
        , ORANGE_FUR = 2
        , WHITE_SQUARES = 3
        , BLUE_SQUARES = 4
        , TAN_SWIRLS = 5
        , NONE
    };

    struct TileData {
        TileID id = TileID::WHITE_MARBLE;
        Fastboi::Rect cutout = Fastboi::Rect(0, 0, 0, 0);


        static const TileData& Get(TileID id);
    };

    void InitTileData();
}