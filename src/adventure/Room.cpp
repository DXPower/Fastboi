#include "Room.h"
#include "Level.h"
#include <cmath>
#include "FastboiComps.h"

using namespace Adventure;

Room::Room(const Layouts::Layout_t& layout)
     : coords(layout.GetCoords(Level::gcCoords)), originalColor(layout.color), currentColor(layout.color)
     , origin(Level::origin + size * (Position) coords), neighbors(layout.neighbors) {
    
    tiles.reserve(layout.tiles.size());
    size_t y = 0;
    
    for (const std::string& row : layout.tiles) {
        std::vector<Gameobject*>& tileRow = tiles.emplace_back(row.size(), nullptr);

        for (size_t x = 0; x < row.size(); x++) {
            switch (row[x]) {
                case WALL: {
                    Gameobject& tile = Instantiate<Tile>(*this, Veci(x, y), layout.color);
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

BoundingBox Room::GetBounds() const {
    BoundingBox b;
    b.lowerBounds = GetTilePos(0, 0) - Room::GetTileSize() / 2.f;
    b.upperBounds = b.lowerBounds + GetSize();

    return b;
}

Position Room::GetRoomCenter(const Vec<int>& roomCoords) {
    return Level::origin + size * (Position) roomCoords + size / 2.f - tileSize / 2.f;
}

Position Room::GetRoomCenterFromWorldPos(const Position& worldPos) {
    Position p = (worldPos - Level::origin) / size;
    Vec<int> roomCoords(std::floor(p.x), std::floor(p.y));

    return GetRoomCenter(roomCoords);
}

void Room::SetColor(const ColorComp& c) {
    currentColor = c;

    for (const auto& row : tiles) {
        for (Gameobject* tile : row) {
            if (tile == nullptr) continue;
            
            tile->GetComponent<ColorComp>() = c;
        }
    }
}

void Adventure::Tile(Gameobject& go, const Room& room, const Veci index, const ColorComp& color) {
    go.name = "Wall";
    go.AddComponent<Transform>(room.GetTilePos(index), Size(room.GetTileSize()), 0);
    go.AddComponent<BoxColorRenderer>(RenderData(RenderOrder::GROUND));
    go.AddComponent<Collider>(Collider::FIXED, CollisionLayer::WALLS).mask.Include(CollisionLayer::PLAYER);
    go.AddComponent<ColorComp>(color);
}