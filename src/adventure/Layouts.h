#pragma once

#include <array>
#include "ColorComp.h"
#include <initializer_list>
#include <string>
#include "Vec.h"

namespace Adventure::Layouts { 
    struct Layout_t {
        std::initializer_list<const std::string> tiles;
        Fastboi::Vec<int> gcOffset;
        Fastboi::Components::ColorComp color;

        enum Dirs {
            UP = 0, RIGHT = 1, DOWN = 2, LEFT = 3
        };

        using VI = Fastboi::Vec<int>;
        inline static constexpr VI dfltUp{0, -1};
        inline static constexpr VI dfltRight{1, 0}; 
        inline static constexpr VI dfltDown{0, 1};
        inline static constexpr VI dfltLeft{-1, 0};
        std::array<VI, 4> neighbors = { dfltUp, dfltRight, dfltDown, dfltLeft };

        Fastboi::Vec<int> GetCoords(const Fastboi::Vec<int>& goldCastleCoords) const;
    };

    extern const Layout_t goldCastle;
    extern const Layout_t goldCastleInside;
    extern const Layout_t gcSouth;
    extern const Layout_t gcSoutheast;
    extern const Layout_t gcSouthwest;
    extern const Layout_t greenDragonRm;
    extern const Layout_t mazeU;
    extern const Layout_t mazeSwirls;
    extern const Layout_t mazeHumps;
    extern const Layout_t mazeBase;
    extern const Layout_t mazeGrail;
    extern const Layout_t blackCastle;
    extern const Layout_t blackCastleInsideBot;
    extern const Layout_t blackCastleInsideTop;

    extern const Fastboi::Vec<int> gcCenterL1;
    extern const Fastboi::Vec<int> gcCenterL2;
}