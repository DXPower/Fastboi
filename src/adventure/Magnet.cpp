#include "Magnet.h"
#include "FastboiComps.h"
#include "Item.h"
#include "Room.h"
#include "Chalise.h"

using namespace Adventure;

Magnet::Magnet(GORef&& go) : go(std::move(go)) { }

void Magnet::Start() {
    itemSelf = &go().GetComponent<Item>();
}

static constexpr float pullSpeed = 100.f;

void Magnet::Update() {
    // Pull other items in
    for (const Item* item : Item::GetAllItems()) {
        if (item->room == itemSelf->room // Only pull items in the same room
            && !item->isHeld // Don't pull any item the player is holding
            && item != itemSelf // Don't pull itself
            && !item->go().HasComponent<Chalise>()) { // Chalise isn't magnetic

            Transform& pull = *item->go().transform;
            const Transform& magnet = *go().transform;

            Position pullDest = magnet.position;
            pullDest.y += magnet.size.y / 2.f + pull.size.y / 2.f;

            Vecf dir = pullDest - pull.position;

            if (dir.magnitude() < 0.9f) continue;

            pull.position += dir.normalized() * pullSpeed * Fastboi::tickDelta;
        }
    }
}

void Magnet::Inst(Gameobject& go, const Position& pos) {
    constexpr Vec<int> magnetSpriteSize{ 16, 16 };

    go.AddComponent<Transform>(pos, Room::GetTileSize(), 0);
    go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::OBJECTS_OVER), "Magnet", Rect(0, 0, magnetSpriteSize.x, magnetSpriteSize.y));
    go.AddComponent<Collider>(Collider::TRIGGER, CollisionLayer::ITEMS).mask.Include(CollisionLayer::PLAYER);
    
    go.AddComponent<Item>();
    go.AddComponent<Magnet>();
}