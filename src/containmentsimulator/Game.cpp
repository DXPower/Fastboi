#include "Game.h"
#include "Button.h"
#include "CameraController.h"
#include "Fastboi.h"
#include "FastboiComps.h"
#include <random>
#include "FastboiCore.h"
#include "Resources.h"
#include "Tile.h"
#include "TileData.h"
#include "WorldEditor.h"

using namespace Fastboi;
using namespace Fastboi::Components;
using namespace CS;

std::random_device rd;

void CreateLayers();

void CS::LoadResources() {
    Resources::LoadImage("TileSpritesheet", "tiles.png");

    const Texture& tiles = Resources::GetTexture("TileSpritesheet");
    SDL_SetTextureBlendMode(tiles.GetSDL_Texture(), SDL_BLENDMODE_BLEND);

    InitTileData();
}

void CS::InitGame() {
    Fastboi::Print("Hello, world!\n");

    LoadResources();

    Gameobject& mainCamera = Instantiate<CameraController::Blueprint>();

    CreateLayers();
    Gameobject& worldEditor = Instantiate<WorldEditor::Blueprint>(Position(0.25, 0.9));

}

void CreateLayers() {
    std::uniform_int_distribution<int> grassRotDist(0, 3);
    std::uniform_int_distribution<int> grassDistX(0, 4);
    std::uniform_int_distribution<int> grassDistY(0, 1);

    Gameobject& groundGO = Instantiate<TileLayer::Blueprint>(Position(25, 20), Veci(15, 10), Veci(64, 64));
    TileLayer& ground = groundGO.GetComponent<TileLayer>();
    Rect grassOrigin = Rect(960, 640, 64, 64);

    ground.LockTexture();

    for (Tile& tile : ground.GetTiles()) {
        tile.SetCutout(Rect(grassOrigin.x + grassDistX(rd) * 64, grassOrigin.y + grassDistY(rd) * 64, 64, 64));
        tile.SetRotation(90_deg * grassRotDist(rd));
    }

    ground.UnlockTexture();
    ground.UpdateTexture();
}