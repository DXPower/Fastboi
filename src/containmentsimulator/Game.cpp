#include "Game.h"
#include "CameraController.h"
#include "Fastboi.h"
#include "FastboiComps.h"
#include "Resources.h"
#include "Tile.h"

using namespace Fastboi;
using namespace Fastboi::Components;

void CS::LoadResources() {
    Resources::LoadImage("TileSpritesheet", "tiles.png");
}

void CS::InitGame() {
    Fastboi::Print("Hello, world!\n");

    LoadResources();


    Gameobject& layer = Instantiate<TileLayer::Blueprint>(Position(25, 20), Veci(15, 10), Veci(64, 64));

    Gameobject& mainCamera = Instantiate<CameraController::Blueprint>();

    // Gameobject& tile = Instantiate<Gameobject>("Tile Test");
    
    // tile.AddComponent<Transform>(Position(10, 10), Size(400, 400), 0_deg);
    // // tile.AddComponent<BoxColorRenderer>(RenderData(RenderOrder::GROUND, 0));
    // // tile.AddComponent<ColorComp>(255, 0, 0, 255);
    // tile.AddComponent<SpriteRenderer>(RenderData(RenderOrder::GROUND, 0), "TileSpritesheet", Rect(1472, 512, 64, 64));
    

}