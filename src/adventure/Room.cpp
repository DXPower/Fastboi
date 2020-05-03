#include "Room.h"
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

Position Room::GetRoomCenterFromWorldPos(const Position& worldPos) {
    Vec<int> roomCoords = (worldPos - roomGlobalOrigin) / size;
    // printf("Room coords: %i %i\n", roomCoords.x, roomCoords.y);
    return roomGlobalOrigin + size * (Position) roomCoords + size / 2.f - tileSize / 2.f;
}

void Adventure::Tile(Gameobject& go, const Room& room, const Veci index, const ColorComp& color) {
    go.name = "Wall";
    go.AddComponent<Transform>(room.GetTilePos(index), Size(room.GetTileSize()), 0);
    go.AddComponent<BoxColorRenderer>(RenderData(RenderOrder::GROUND));
    go.AddComponent<Collider>(Collider::FIXED);
    go.AddComponent<ColorComp>(color);
}