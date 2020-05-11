#include "Bridge.h"
#include "FastboiComps.h"
#include "Item.h"
#include "Player.h"
#include "Room.h"

using namespace Adventure;

Bridge::Bridge(Fastboi::GORef&& go) : go(std::move(go)) {
}

void Bridge::Start() {
    go().collider->collisionSignal.connect<&Bridge::EnterBridge>(this);
}

void Bridge::Update() {

}

void Bridge::EnterBridge(const CollisionEvent& e) {
    if (e.collider.gameobject().name == "Player") {
        if (e.type == CollisionEvent::BEGIN)
            e.collider.gameobject().GetComponent<Player>().EnterBridge();
        else
            e.collider.gameobject().GetComponent<Player>().ExitBridge();
    }
}

void BridgePart(Gameobject& go, Item& itemParent, const Position& pos, const Size& size) {
    go.AddComponent<Transform>(pos, size, 0).Parent(itemParent.go().transform);
    // go.AddComponent<ColorComp>(0, 255, 0, 255);
    // go.AddComponent<WireframeRenderer>(RenderData(RenderOrder::UI, 10));
    
    Collider& col = go.AddComponent<Collider>(Collider::TRIGGER, CollisionLayer::ITEMS);
    col.mask.Include(CollisionLayer::PLAYER);
    col.collisionSignal.connect<&Item::TryPickup>(itemParent);
}

void BridgeInternal(Gameobject& go, Bridge& bridgeParent, const Position& pos, const Size& size) {
    go.AddComponent<Transform>(pos, size, 0).Parent(bridgeParent.go().transform);
    // go.AddComponent<ColorComp>(255, 155, 0, 255);
    // go.AddComponent<WireframeRenderer>(RenderData(RenderOrder::UI, 9));

    Collider& col = go.AddComponent<Collider>(Collider::TRIGGER, CollisionLayer::ITEMS);
    col.mask.Include(CollisionLayer::PLAYER);
    col.collisionSignal.connect<&Bridge::EnterBridge>(bridgeParent);
}

void Bridge::Inst(Gameobject& go, const Position& pos) {
    constexpr Vec<int> bridgeSpriteSize{64, 48};
    constexpr Size bridgeSize{ (float) bridgeSpriteSize.x / (float) bridgeSpriteSize.y * 3 * Room::GetTileSize().y, 3 * Room::GetTileSize().y };

    go.AddComponent<Transform>(pos, bridgeSize, 0);
    go.AddComponent<Collider>(Collider::TRIGGER, CollisionLayer::ITEMS).mask.Include(CollisionLayer::PLAYER);
    go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::OBJECTS_OVER, -1), "Bridge", Rect(0, 0, bridgeSpriteSize.x, bridgeSpriteSize.y));

    Item& item = go.AddComponent<Item>();
    Bridge& bridge = go.AddComponent<Bridge>();
    
    Instantiate<BridgeInternal>(bridge, pos, bridgeSize * Vecf(0.5f, 1.f));

    constexpr Size feetSize{ bridgeSize.x / 8.f, bridgeSize.y / 6.f };
    BoundingBox bs = go.transform->GetBounds();

    Instantiate<BridgePart>(item, pos + bridgeSize * Vecf(-0.32f, 0), bridgeSize * Vecf(8.f / bridgeSpriteSize.x, 1.f));
    Instantiate<BridgePart>(item, pos + bridgeSize * Vecf(0.32f, 0), bridgeSize * Vecf(8.f / bridgeSpriteSize.x, 1.f));
    Instantiate<BridgePart>(item, bs.lowerBounds + feetSize / 2.f, feetSize);
    Instantiate<BridgePart>(item, bs.upperBounds - feetSize / 2.f, feetSize);
    Instantiate<BridgePart>(item, Position(bs.lowerBounds.x + feetSize.x / 2.f, bs.upperBounds.y - feetSize.y / 2.f), feetSize);
    Instantiate<BridgePart>(item, Position(bs.upperBounds.x - feetSize.x / 2.f, bs.lowerBounds.y + feetSize.y / 2.f), feetSize);
}