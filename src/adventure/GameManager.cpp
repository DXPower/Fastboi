#include "GameManager.h"
#include <random>

using namespace Adventure;

std::random_device rd;
std::uniform_int_distribution<int> dist(0, 99);
std::uniform_real_distribution<float> pushDistanceDist(1.2f, 3.f);
std::uniform_real_distribution<float> grundlePushDistanceDist(1.f, 2.f);

Position OrientedDragonEntrance(const Room& room, Gameobject& dragon) {
    Vecf roomOffset = (Vecf) room.coords - (Vecf) Level::GetRoom(dragon.transform->position).coords;

    // So if player moved up, then roomOffset = (0, -1)
    // Take room center, subtract roomOffset * size / 2 - roomOffset * size * grundlePush
    float grundlePush = grundlePushDistanceDist(rd);
    return room.GetCenter() - roomOffset * room.GetSize() / 2.f - roomOffset * dragon.transform->size * grundlePush;
}

Position RandomDragonEntrance(const Room& room, Gameobject& dragon) {
    const int corner = dist(rd) % 8;

    Position cPos = room.origin; // Corner position
    Vec<int> pushDir; // Direction to push dragon a variable amount

    switch (corner) {
        case 0: // Top left
            pushDir = Vec<int>(-1, -1);
            break;
        case 1: // Top right
            cPos += room.GetSize() * Size(1.f, 0.f);
            pushDir = Vec<int>(1, -1);
            break;
        case 2: // Bottom left
            cPos += room.GetSize() * Size(0.f, 1.f);
            pushDir = Vec<int>(-1, 1);
            break;
        case 3: // Bottom right
            cPos += room.GetSize();
            pushDir = Vec<int>(1, 1);
            break;
        case 4: // Top
            cPos += room.GetSize() * Size(0.5f, 0.f);
            pushDir = Vec<int>(0, -1);
            break;
        case 5: // Right
            cPos += room.GetSize() * Size(1.f, 0.5f);
            pushDir = Vec<int>(1, 0);
            break;
        case 6: // Left
            cPos += room.GetSize() * Size(0.f, 0.5f);
            pushDir = Vec<int>(-1, 0);
            break;
        default: // Bottom
            cPos += room.GetSize() * Size(0.5f, 1.f);
            pushDir = Vec<int>(0, 1);
            break;
    }

    const float pushAwayDist = pushDistanceDist(rd);

    return cPos + dragon.transform->size * ((Position) pushDir) * pushAwayDist;
}

void GameManager::MoveDragons(const Room& room) {
    if (yorgle != nullptr && !yorgle->IsDead()) {
        if (!hasSeenYorgle) {
            if (yorgle->room == &room) {
                hasSeenYorgle = true;

                yorgle->SetChase(true);
                yorgle->go().Enable();
            }
        } else if (int chance = dist(rd); chance < 50 && !room.banYorgle) {
            yorgle->go().transform->position = RandomDragonEntrance(room, yorgle->go());
            yorgle->go().Enable();
        } else {
            yorgle->go().Disable();
        }
    }

    if (grundle != nullptr && !grundle->IsDead()) {
        if (grundle->room == &room) { // If player enters the room grundle is in (where the key is)
            grundle->SetChase(true);
            grundle->go().Enable();
        } else if (blackKey != nullptr) {
            if (!room.banGrundle) {
                if (blackKey->isHeld) { // If the player has carried the key to another room,
                    grundle->go().transform->position = OrientedDragonEntrance(room, grundle->go());
                    grundle->go().Enable();
                } else { // If the player leaves the room without the key
                    grundle->go().Disable();
                    grundle->go().transform->position = blackKey->go().transform->position;
                    grundle->room = &Level::GetRoom(grundle->go().transform->position);
                }
            } else {
                grundle->go().Disable();
            }
        }
    } 
}

void GameManager::RoomChanged(const RoomChangeEvent& e) {
    MoveDragons(e.room);
}

void GameManager::WinGame() const {
    printf("Winner!\n");
    Level::GetPlayer().DisableComponent<Player>(); // Stop player from moving//inputting
    Level::GetPlayer().DisableComponent<Rigidbody>();   
}