#include "Game.h"
#include "Chalise.h"
#include "Dragon.h"
#include "GameManager.h"
#include "Gate.h"
#include "Item.h"
#include "Layouts.h"
#include "Level.h"
#include "Magnet.h"
#include "Player.h"
#include "Room.h"

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
    Fastboi::Resources::LoadImage("Castle", "castle.png");
    Fastboi::Resources::LoadImage("Yorgle", "yorgle.png");
    Fastboi::Resources::LoadImage("Grundle", "grundle.png");
    Fastboi::Resources::LoadImage("Rhindle", "rhindle.png");
    Fastboi::Resources::LoadImage("Chalise", "chalise.png");
    Fastboi::Resources::LoadImage("Magnet", "magnet.png");

    printf("Resources loaded\n");
}

void Adventure::InitGame() {
    Rendering::SetBGColor(ColorComp(170, 170, 170, 255));

    LoadResources();
    LoadLevel1();

    Fastboi::camera.zoom = 0.49f;
    Level::roomChangeSignal.connect<&GameManager::RoomChanged>(manager);
}

constexpr ColorComp mazeBlue(66, 72, 200, 255);

void Adventure::LoadLevel1() {
    Level::SetLevelSize(5, 7);
    Level::gcCoords = Vec<int>(3, 4);

    Room& goldCastle = Level::AddRoom(Layouts::goldCastle);
    Room& goldCastleInside = Level::AddRoom(Layouts::goldCastleInside);
    manager.goldCastleInside = &goldCastleInside;

    Room& gcSouth = Level::AddRoom(Layouts::gcSouth);
    Room& gcSoutheast = Level::AddRoom(Layouts::gcSoutheast);
    Room& gcSouthwest = Level::AddRoom(Layouts::gcSouthwest);
    Room& greenDragonRm = Level::AddRoom(Layouts::greenDragonRm);
    Room& mazeU = Level::AddRoom(Layouts::mazeU);
    Room& mazeSwirls = Level::AddRoom(Layouts::mazeSwirls);
    Room& mazeHumps = Level::AddRoom(Layouts::mazeHumps);
    Room& mazeBase = Level::AddRoom(Layouts::mazeBase);
    Room& mazeGrail = Level::AddRoom(Layouts::mazeGrail);
    Room& blackCastle = Level::AddRoom(Layouts::blackCastle);
    Room& blackCastleInsideBot = Level::AddRoom(Layouts::blackCastleInsideBot);
    Room& blackCastleInsideTop = Level::AddRoom(Layouts::blackCastleInsideTop);

    Gameobject& player = Instantiate<Player::Inst>(goldCastle.GetTilePos(Vec<int>(9, 9)));
    Gameobject& sword = Instantiate<Sword::Inst>(goldCastleInside.GetTilePos(Vec<int>(3, 9)));
    Gameobject& chalise = Instantiate<Chalise::Inst>(goldCastle.GetTilePos(7, 9));
    Gameobject& magnet = Instantiate<Magnet::Inst>(goldCastle.GetTilePos(14, 9));

    Gameobject& goldKey = Instantiate<Key::Inst>(goldCastle.GetTilePos(Vec<int>(4, 5)), KeyColor::GOLD);
    Gameobject& blackKey = Instantiate<Key::Inst>(greenDragonRm.GetTilePos(Vec<int>(4, 4)), KeyColor::BLACK);

    Gameobject& goldGate = Instantiate<Gate::Inst>(
        goldCastle.GetTilePos(Vec<int>(9, 7)) + Room::GetTileSize() / 2.f
        , KeyColor::GOLD
        , goldCastleInside
    );
    
    Gameobject& blackGate = Instantiate<Gate::Inst>(
        blackCastle.GetTilePos(Vec<int>(9, 7)) + Room::GetTileSize() / 2.f
        , KeyColor::BLACK
        , blackCastleInsideBot
    );


    Gameobject& gldCastle = Instantiate<Castle::Inst>(
        goldCastle.GetTilePos(Vec<int>(10, 4)) - Position(Room::GetTileSize().x / 2.f, 8.f)
        , KeyColor::GOLD
    );

    Gameobject& blckCastle = Instantiate<Castle::Inst>(
        blackCastle.GetTilePos(Vec<int>(10, 4)) - Position(Room::GetTileSize().x / 2.f, 8.f)
        , KeyColor::BLACK
    );

    Gameobject& yorgle = Instantiate<Yorgle::Inst>(player, gcSouthwest, gcSouthwest.GetTilePos(Vec<int>(4, 5)));
    Gameobject& grundle = Instantiate<Grundle::Inst>(player, greenDragonRm, greenDragonRm.GetTilePos(Vec<int>(10, 7)));

    goldCastle.banYorgle = true;
    goldCastle.banGrundle = true;
    goldCastleInside.banYorgle = true;
    goldCastleInside.banGrundle = true;

    mazeU.banGrundle = true;
    mazeSwirls.banGrundle = true;
    mazeHumps.banGrundle = true;
    mazeBase.banGrundle = true;
}