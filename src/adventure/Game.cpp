#include "Game.h"
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
    // Fastboi::Resources::LoadImage("Player", "penguin.png");
    // Fastboi::Resources::LoadImage("Brick", "brick.png");
    // Fastboi::Resources::LoadImage("Button", "button.png");
    // Fastboi::Resources::LoadImage("Spritemap", "spritemap.png");
    Fastboi::Resources::LoadImage("Keys", "keys.png");
}

Room* firstRoom = nullptr;
Room* secondRoom = nullptr;

void Adventure::InitGame() {
    LoadResources();

    // Gameobject& anon = Instantiate<Gameobject>("Test object\n");
    // anon.AddComponent<Transform>(Position(100, 200), Size(100, 400), 45);
    // anon.AddComponent<WireframeRenderer>(RenderData(RenderOrder::GROUND, -1));
    // anon.AddComponent<ColorComp>(255, 50, 0, 255);

    firstRoom = new Room(
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
        Vec<int>(0, 0), ColorComp(34, 110, 52, 255)
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
        Vec<int>(0, 1), ColorComp(34, 110, 52, 255)
    );
    
    Gameobject& player = Instantiate<PlayerGO>(firstRoom->GetTilePos(Vec<int>(10, 9)));
    Gameobject& key = Instantiate<KeyGO>(firstRoom->GetTilePos(Vec<int>(4, 5)), Key::GOLD);

    Fastboi::SetCamera(Camera(*player.transform, Camera::WATCHING));
    Fastboi::camera.zoom = 0.5f;
}