#include "Chalise.h"
#include "FastboiComps.h"
#include "Item.h"
#include "Level.h"
#include "GameManager.h"

using namespace Adventure;
using namespace Fastboi::Components;


Chalise::Chalise(GORef&& go, ColorComp& color) : go(std::move(go)), c(color) {
    Level::roomChangeSignal.connect<&Chalise::RoomChanged>(this);
}

void Chalise::Update() {
    static int s = 0;
    static int n = 0;
    static uint8_t rollingShift = 0;

    if (s == 0) 
        c.g += 5, c.b += 5;
    else if (s == 1)
        c.r += 5, c.g += 5;

    else if (s == 2)
        c.r += 5, c.b += 5;

    n += 5;
    if (n >= 255) {
        n = 0;
        s++;
        rollingShift += 25;

        if (s == 3)
            s = 0;

        if (s == 0)
            c.g = 0, c.b = 0, c.r = rollingShift;
        else if (s == 1)
            c.r = 0, c.g = 0, c.b = rollingShift;
        else if (s == 2)
            c.r = 0, c.b = 0, c.g = rollingShift;
    }

    const Texture& tex = go().GetComponent<SpriteRenderer>().texture;

    SDL_SetTextureColorMod(tex.GetSDL_Texture(), c.r, c.g, c.b);

    if (playWinSequence) {
        Level::GetRoom(go().transform->Parent().position).SetColor(c);
        go().transform->Parent().gameobject().GetComponent<ColorComp>() = c;
    }
        
}

void Chalise::RoomChanged(const RoomChangeEvent& e) {
    if (go().GetComponent<Item>().isHeld && &Level::GetRoom(go().transform->Parent().position) == manager.goldCastleInside) {
        manager.WinGame();

        playWinSequence = true;
    }
}

void Chalise::Inst(Gameobject& go, const Position& pos) {
    constexpr Vec<int> chaliseSpriteSize{16, 18};
    constexpr Size chaliseSize{Room::GetTileSize().x, (float) chaliseSpriteSize.y / (float) chaliseSpriteSize.x * Room::GetTileSize().x};

    go.AddComponent<Transform>(pos, chaliseSize, 0_deg);
    go.AddComponent<Collider>(Collider::TRIGGER, CollisionLayer::ITEMS).mask.Include(CollisionLayer::PLAYER);

    ColorComp& color = go.AddComponent<ColorComp>(0, 0, 0, 255);
    go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::OBJECTS_OVER), "Chalise", Rect(0, 0, chaliseSpriteSize.x, chaliseSpriteSize.y));

    go.AddComponent<Item>();
    go.AddComponent<Chalise>(color);
}