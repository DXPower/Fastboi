#pragma once

#include "Fastboi.h"
#include "Layouts.h"
#include <initializer_list>
#include <string>

namespace Adventure {
    using namespace Fastboi;
    using namespace Fastboi::Components;

    struct Room {
        private:
        const ColorComp originalColor;
        ColorComp currentColor;

        std::vector<std::vector<Gameobject*>> tiles;

        static inline constexpr Size tileSize{100.f, 100.f};
        static inline constexpr Veci roomTileCount{20, 12};
        static inline constexpr Size size{tileSize * (Size) roomTileCount};

        public:
        bool banYorgle = false;
        bool banGrundle = false;
        bool banRhindle = false;

        const Vec<int> coords;
        const Position origin;
        const decltype(Layouts::Layout_t::neighbors) neighbors;

        Room(const Layouts::Layout_t& layout);
        
        void SetColor(const ColorComp& color);\
        inline const ColorComp& GetColor() const { return currentColor; };

        inline Position GetTilePos(int x, int y) const { return GetTilePos(Vec<int>(x, y)); };
        Position GetTilePos(const Vec<int>& coords) const;
        Position GetCenter() const;

        BoundingBox GetBounds() const;

        //TODO: Replace constexpr with consteval once intellisense shuts up
        inline static constexpr Size GetSize() { return size; };
        inline static constexpr Size GetTileSize() { return tileSize; };
        inline static constexpr Vec<int> GetTileCount() { return roomTileCount; };

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