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
    printf("Resources loaded\n");
}

Room* goldCastle = nullptr;
Room* goldCastleInside = nullptr;

Room* secondRoom = nullptr;

void Adventure::InitGame() {
    Rendering::SetBGColor(ColorComp(170, 170, 170, 255));

    LoadResources();

    goldCastle = new Room(
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

    secondRoom = new Room(
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
        Vec<int>(0, 1), ColorComp(92, 186, 92, 255)
    );
    
    Gameobject& player = Instantiate<PlayerGO>(goldCastle->GetTilePos(Vec<int>(10, 9)));
    Gameobject& key = Instantiate<KeyGO>(goldCastle->GetTilePos(Vec<int>(4, 5)), KeyColor::GOLD);
    Gameobject& gate = Instantiate<Gate::Inst>(goldCastle->GetTilePos(Vec<int>(10, 6)), KeyColor::GOLD, *goldCastleInside);

    Fastboi::SetCamera(Camera(*player.transform, Camera::WATCHING));
    Fastboi::camera.zoom = 0.5f;
}