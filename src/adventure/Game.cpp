#include "Game.h"
#include "Gate.h"
#include "Item.h"
#include "Room.h"
#include "Player.h"

using namespace Fastboi;
using namespace Fastboi::Components;
using namespace Adventure;

Fastboi::Input::KeyListener pauseListener(SDL_SCANCODE_ESCAPE);
Input::KeyListener aabbListener(SDL_SCANCODE_F1);

void TogglePause(const Fastboi::KeyEvent& e) {
    if (e.type == KeyEvent::DOWN) {
        if (Fastboi::IsPaused())
            Fastboi::Unpause();
        else
            Fastboi::Pause();
    }
}

void ToggleAABBDebug(const Fastboi::KeyEvent& e) {
    static bool enableAABBs = false;

    if (e.type == KeyEvent::DOWN) {
        enableAABBs = !enableAABBs;
        Collision::SetRenderAABBTree(enableAABBs);
    }
}

void LoadResources() {
    Fastboi::Resources::LoadImage("Keys", "keys.png");
    Fastboi::Resources::LoadImage("Gate", "gate.png");
    Fastboi::Resources::LoadImage("Sword", "sword.png");
    printf("Resources loaded\n");
}

void Adventure::InitGame() {
    Rendering::SetBGColor(ColorComp(170, 170, 170, 255));

    LoadResources();
    LoadLevel1();

    Fastboi::camera.zoom = 0.5f;
}


Room* goldCastle = nullptr;
Room* goldCastleInside = nullptr;
Room* gcSouth = nullptr;
Room* gcSoutheast = nullptr;
Room* gcSouthwest = nullptr;
Room* greenDragonRm = nullptr;
Room* mazeU = nullptr;
Room* mazeSwirls = nullptr;

void Adventure::LoadLevel1() {
    goldCastle = new Room(
        {
            "WWWWWWWWWWWWWWWWWWWW",
            "WOOOOWOOOOOOOOWOOOOW",
            "WOOOOWOOOOOOOOWOOOOW",
            "WOOOOWOOOOOOOOWOOOOW",
            "WOOOOWOOOOOOOOWOOOOW",
            "WOOOOOWOOOOOOWOOOOOW",
            "WOOOOOWWWWWWWWOOOOOW",
            "WOOOOOWWWOOWWWOOOOOW",
            "WOOOOOWWWOOWWWOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WWWWWWWWOOOOWWWWWWWW"
        },
        Vec<int>(0, 0), ColorComp(210, 210, 64, 255)
    );

    goldCastleInside = new Room(
        {
            "WWWWWWWWWWWWWWWWWWWW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WWWWWWWWOOOOWWWWWWWW"
        },
        Vec<int>(0, -1), ColorComp(210, 210, 64, 255)
    );

    gcSouth = new Room(
        {
            "WWWWWWWWOOOOWWWWWWWW",
            "OOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOO",
            "WWWWWWWWWWWWWWWWWWWW"
        },
        Vec<int>(0, 1), ColorComp(92, 186, 92, 255)
    );
    
    gcSoutheast = new Room(
        {
            "WWWWWWWWWWWWWWWWWWWW",
            "OOOOOOOOOOOOOOOOOOOW",
            "OOOOOOOOOOOOOOOOOOOW",
            "OOOOOOOOOOOOOOOOOOOW",
            "OOOOOOOOOOOOOOOOOOOW",
            "OOOOOOOOOOOOOOOOOOOW",
            "OOOOOOOOOOOOOOOOOOOW",
            "OOOOOOOOOOOOOOOOOOOW",
            "OOOOOOOOOOOOOOOOOOOW",
            "OOOOOOOOOOOOOOOOOOOW",
            "OOOOOOOOOOOOOOOOOOOW",
            "WWWWWWWWOOOOWWWWWWWW"
        },
        Vec<int>(1, 1), ColorComp(160, 171, 79, 255)
    );
    
    gcSouthwest = new Room(
        {
            "WWWWWWWWOOOOWWWWWWWW",
            "WOOOOOOOOOOOOOOOOOOO",
            "WOOOOOOOOOOOOOOOOOOO",
            "WOOOOOOOOOOOOOOOOOOO",
            "WOOOOOOOOOOOOOOOOOOO",
            "WOOOOOOOOOOOOOOOOOOO",
            "WOOOOOOOOOOOOOOOOOOO",
            "WOOOOOOOOOOOOOOOOOOO",
            "WOOOOOOOOOOOOOOOOOOO",
            "WOOOOOOOOOOOOOOOOOOO",
            "WOOOOOOOOOOOOOOOOOOO",
            "WWWWWWWWWWWWWWWWWWWW"
        },
        Vec<int>(-1, 1), ColorComp(135, 183, 84, 255)
    );

    greenDragonRm = new Room(
        {
            "WWWWWWWWOOOOWWWWWWWW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WOOOOOOOOOOOOOOOOOOW",
            "WWWWWWWWWWWWWWWWWWWW"
        },
        Vec<int>(1, 2), ColorComp(198, 108, 58, 255)
    );
    
    mazeU = new Room(
        {
            "WWWWOWOWOWWOWOWOWWWW",
            "OOOWOWOWOOOOWOWOWOOO",
            "OOOWOWOWOOOOWOWOWOOO",
            "WWOWOWOWWWWWWOWOWOWW",
            "WWOWOWOWWWWWWOWOWOWW",
            "OOOWOWOOOOOOOOWOWOOO",
            "OOOWOWOOOOOOOOWOWOOO",
            "WWWWOWWWWWWWWWWOWWWW",
            "WWWWOWWWWWWWWWWOWWWW",
            "OOOOOOOOOOOOOOOOOOOO",
            "OOOOOOOOOOOOOOOOOOOO",
            "WWWWWWWWOOOOWWWWWWWW"
        },
        Vec<int>(-1, 0), ColorComp(66, 72, 200, 255)
    );

    mazeSwirls = new Room(
        {
            "WWOWOWWWWOOWWWWOWOWW",
            "OOOWOOOWWOOWWOOOWOOO",
            "OOOWOOOWWOOWWOOOWOOO",
            "WWWWWWOWWOOWWOWWWWWW",
            "WWWWWWOWWOOWWOWWWWWW",
            "OOOOOWOWWOOWWOWOOOOO",
            "OOOOOWOWWOOWWOWOOOOO",
            "WWOWOWOWWOOWWOWOWOWW",
            "WWOWOWOWWOOWWOWOWOWW",
            "OOOWOWOWOOOOWOWOWOOO",
            "OOOWOWOWOOOOWOWOWOOO",
            "WWWWOWOWOOOOWOWOWWWW"
        },
        Vec<int>(-2, 0), ColorComp(66, 72, 200, 255)
    );

    
    
    Gameobject& player = Instantiate<Player::Inst>(goldCastle->GetTilePos(Vec<int>(9, 9)));
    Gameobject& key = Instantiate<Key::Inst>(goldCastle->GetTilePos(Vec<int>(4, 5)), KeyColor::GOLD);
    Gameobject& gate = Instantiate<Gate::Inst>(
        goldCastle->GetTilePos(Vec<int>(9, 7)) + Room::GetTileSize() / 2.f
        , KeyColor::GOLD
        , *goldCastleInside
    );

    Gameobject& sword = Instantiate<Sword::Inst>(goldCastleInside->GetTilePos(Vec<int>(3, 9)));
}