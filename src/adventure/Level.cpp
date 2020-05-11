#include "Level.h"

using namespace Adventure;
Room& Level::AddRoom(const Layouts::Layout_t& layout) {
    int x = layout.GetCoords(Level::gcCoords).x;
    int y = layout.GetCoords(Level::gcCoords).y;

    printf("Making room in x,y %i %i...\n", x, y);
    rooms[y][x] = std::make_unique<Room>(layout);
    printf("Made room there\n");
    return *rooms[y][x];
}

Room& Level::GetRoom(const Position& pos) {
    const Position p = (pos - (Level::origin - Room::GetTileSize() / 2.f)) / Room::GetSize();
    const Vec<int> coords(std::floor(p.x), std::floor(p.y)); 

    return *rooms[coords.y][coords.x];
}

int num = 0;

// Takes into account the current room (if player is trying to move between screens)
Room& Level::GetRoomMovement(const Position& pos, const Velocity& v, Vec<int>& dirMovedOut) {
    Room& roomMajor = GetRoom(pos); // The room the player is in spatially

    BoundingBox roomBounds = roomMajor.GetBounds();
    roomBounds.lowerBounds += Room::GetTileSize() / Size(4.f, 2.f);
    roomBounds.upperBounds -= Room::GetTileSize() / Size(4.f, 2.f);

    roomBounds = roomBounds.Fatten(-0.0025f);

    using namespace Layouts;

    Vec<int> newRoomOffset;

    if (pos.x < roomBounds.lowerBounds.x && v.x < 0) { // Moving to new room left
        newRoomOffset = roomMajor.neighbors[Layout_t::LEFT];
        dirMovedOut = Layout_t::dfltLeft;
    } else if (pos.x > roomBounds.upperBounds.x && v.x > 0) { // Moving to new room right
        newRoomOffset = roomMajor.neighbors[Layout_t::RIGHT];
        dirMovedOut = Layout_t::dfltRight;
    } else if (pos.y < roomBounds.lowerBounds.y && v.y < 0) { // Moving new room up
        newRoomOffset = roomMajor.neighbors[Layout_t::UP];
        dirMovedOut = Layout_t::dfltUp;
    } else if (pos.y > roomBounds.upperBounds.y && v.y > 0) { // Moving new room down
        newRoomOffset = roomMajor.neighbors[Layout_t::DOWN];
        dirMovedOut = Layout_t::dfltDown;
    } else {// Not moving to a new room
        dirMovedOut = Vec<int>::zero();
        return roomMajor;
    }

    printf("===================== %i\n", num++);
    printf("Cur room: %i %i\n", roomMajor.coords.x, roomMajor.coords.y);
    printf("Room offset: %i %i\n", newRoomOffset.x, newRoomOffset.y);
    printf("New coords: %i %i\n", roomMajor.coords.x + newRoomOffset.x, roomMajor.coords.y + newRoomOffset.y);
    return Level::GetRoom(roomMajor.coords + newRoomOffset);
}

void Level::SetLevelSize(size_t x, size_t y) {
    rooms.resize(y);

    for (auto& v : rooms) {
        v.resize(x);
    }
}

void RoomObserver::Start() {
    rb = &go().GetComponent<Rigidbody>();

    curRoom = &Level::GetRoom(go().transform->position);
    ChangeRoom(*curRoom);
}

void RoomObserver::Update() {
    Transform& tr = *go().transform;

    Vec<int> dirMoved;
    if (Room& newRoom = Level::GetRoomMovement(tr.position, rb->velocity, dirMoved); &newRoom != curRoom) {

        printf("teleporting rooms!\n");
        
        printf("Dir moved: %i %i\n", dirMoved.x, dirMoved.y);

        // Teleport across the screen, keeping same relative position on the axis of teleportation
        // If we down, dirMoved is <0, 1>. This means we have to keep the x position the same, but flip the y along the center
        // axisFlip = keep not changing axis same, flip changing axis
        const Vec<int> axisFlip = Vec<int>(dirMoved.x == 0 ? 1 : -1, dirMoved.y == 0 ? 1 : -1);
        const Vecf playerRoomOffset = tr.position - curRoom->GetCenter();

        tr.position = newRoom.GetCenter() + playerRoomOffset * (Vecf) axisFlip;

        ChangeRoom(newRoom);
        curRoom = &newRoom;
    }
}

void RoomObserver::ForceTeleport(const Position& pos) {
    go().transform->position = pos;

    curRoom = &Level::GetRoom(pos);
    ChangeRoom(*curRoom);
}

void RoomObserver::ChangeRoom(Room& room) {
    Level::roomChangeSignal.fire(RoomChangeEvent{ .room = room, .player = go() });
}