#pragma once

#include "Fastboi.h"
#include <initializer_list>
#include <string>

namespace Adventure {
    using namespace Fastboi;
    using namespace Fastboi::Components;

    struct Room {
        private:
        const ColorComp originalColor;
        std::vector<std::vector<Gameobject*>> tiles;

        static inline constexpr Position roomGlobalOrigin = Position::zero();
        static inline constexpr Size tileSize{100.f, 100.f};
        static inline constexpr Veci roomTileCount{20, 12};
        static inline constexpr Size size{tileSize * (Size) roomTileCount};

        public:
        const Vec<int> coords;
        const Position origin;

        Room(std::initializer_list<std::string> layout, const Vec<int>& coords, const ColorComp& color);
        
        void SetColor(const ColorComp& color);
        Position GetTilePos(const Vec<int>& coords) const;
        Position GetCenter() const;

        inline static consteval Size GetSize() { return size; };
        inline static consteval Size GetTileSize() { return tileSize; };
        static Position GetRoomCenter(const Vec<int>& roomCoords);

        static Position GetRoomCenterFromWorldPos(const Position& worldPos);

        public:
        enum TILE_TYPES {
            WALL = 'W',
            AIR = 'O'
        };
    };

    void Tile(Gameobject& go, const Room& room, const Veci index, const ColorComp& color);
}