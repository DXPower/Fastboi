#include "Room.h"
#include <cmath>
#include "FastboiComps.h"

using namespace Adventure;

Room::Room(std::initializer_list<std::string> layout, const Vec<int>& coords, const ColorComp& color)
     : coords(coords), originalColor(color), origin(roomGlobalOrigin + size * (Position) coords) {
    
    tiles.reserve(layout.size());
    size_t y = 0;
    
    for (const std::string& row : layout) {
        std::vector<Gameobject*>& tileRow = tiles.emplace_back(row.size(), nullptr);

        for (size_t x = 0; x < row.size(); x++) {
            switch (row[x]) {
                case WALL: {
                    Gameobject& tile = Instantiate<Tile>(*this, Veci(x, y), color);
                    tileRow[x] = &tile;
                    break;
                }
                default:
                    break;
            }
        }

        y++;
    }
}

Position Room::GetTilePos(const Vec<int>& index) const {
    return origin + GetTileSize() * (Position) index;
}

Position Room::GetCenter() const {
    return GetRoomCenter(coords);
}

Position Room::GetRoomCenter(const Vec<int>& roomCoords) {
    return roomGlobalOrigin + size * (Position) roomCoords + size / 2.f - tileSize / 2.f;
}

Position Room::GetRoomCenterFromWorldPos(const Position& worldPos) {
    Position p = (worldPos - roomGlobalOrigin) / size;
    Vec<int> roomCoords(std::floor(p.x), std::floor(p.y));

    return GetRoomCenter(roomCoords);
}

void Adventure::Tile(Gameobject& go, const Room& room, const Veci index, const ColorComp& color) {
    go.name = "Wall";
    go.AddComponent<Transform>(room.GetTilePos(index), Size(room.GetTileSize()), 0);
    go.AddComponent<BoxColorRenderer>(RenderData(RenderOrder::GROUND));
    go.AddComponent<Collider>(Collider::FIXED);
    go.AddComponent<ColorComp>(color);
}