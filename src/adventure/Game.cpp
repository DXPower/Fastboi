#include "Game.h"
#include "Bridge.h"
#include "Chalise.h"
#include "Dragon.h"
#include "GameManager.h"
#include "Gate.h"
#include "Item.h"
#include "Layouts.h"
#include "Level.h"
#include "Magnet.h"
#include "Player.h"
#include "Resources.h"
#include "Room.h"
#include "RoomCamera.h"

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
    }
}

void LoadResources() {
    Resources::LoadImage("Keys", "keys.png");
    Resources::LoadImage("Gate", "gate.png");
    Resources::LoadImage("Sword", "sword.png");
    Resources::LoadImage("Yorgle", "yorgle.png");
    Resources::LoadImage("Grundle", "grundle.png");
    Resources::LoadImage("Rhindle", "rhindle.png");
    Resources::LoadImage("Chalise", "chalise.png");
    Resources::LoadImage("Magnet", "magnet.png");
    Resources::LoadImage("Bridge", "bridge.png");

    Resources::LoadImage("GoldCastle", "castle.png");
    Resources::LoadImage("BlackCastle", "castle.png");
    Resources::LoadImage("WhiteCastle", "castle.png");

    Resources::LoadSound("WinGood", "win-good.mp3");
    Resources::LoadSound("Pickup", "pickup.mp3");
    Resources::LoadSound("Drop", "drop.mp3");
    Resources::LoadSound("Chomp", "chomp.mp3");
    Resources::LoadSound("Slay", "slay.mp3");
    Resources::LoadSound("Death", "death.mp3");

    SDL_SetTextureColorMod(Resources::GetTexture("GoldCastle").GetSDL_Texture(), 210, 210, 64);
    SDL_SetTextureColorMod(Resources::GetTexture("BlackCastle").GetSDL_Texture(), 0, 0, 0);
    

    printf("Resources loaded\n");
}

#include "soloud/soloud.h"
#include "soloud/soloud_wav.h"

// SoLoud::Soloud gSoloud; // engine
// SoLoud::Wav gWave; // wav file

void Adventure::InitGame() {
    Rendering::SetBGColor(ColorComp(170, 170, 170, 255));

    LoadResources();
    LoadLevel1();

    Level::roomChangeSignal.connect<&GameManager::RoomChanged>(manager);

    // gSoloud.init(); // init soloud
    // gWave.load("res/sound/win-good.mp3");

    // Wav& winWav = Resources::GetSound("WinGood");
    
    // printf("Sound: %p, gWave: %p\n", winWav.mData, gWave.mData);
    // gSoloud.play(winWav);
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

    Gameobject& roomCamera = Instantiate<Gameobject>("RoomCamera");
    roomCamera.AddComponent<Transform>(Level::GetRoom(player.transform->position).GetCenter());
    roomCamera.AddComponent<RoomCamera>();

    Gameobject& sword = Instantiate<Sword::Inst>(goldCastleInside.GetTilePos(Vec<int>(3, 9)));
    Gameobject& chalise = Instantiate<Chalise::Inst>(blackCastleInsideTop.GetTilePos(7, 9));
    Gameobject& magnet = Instantiate<Magnet::Inst>(blackCastleInsideBot.GetTilePos(16, 9));
    Gameobject& bridge = Instantiate<Bridge::Inst>(mazeGrail.GetTilePos(5, 7) + Room::GetTileSize() * 0.5f);

    Gameobject& goldKey = Instantiate<Key::Inst>(goldCastle.GetTilePos(Vec<int>(4, 5)), KeyColor::GOLD);
    goldKey.name = "GoldKey";

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