#include "Item.h"
#include "FastboiComps.h"
#include "Room.h"
#include "Player.h"

using namespace Adventure;
using namespace Fastboi;
using namespace Fastboi::Components;

Item::Item(GORef&& go) : go(std::move(go)) { };

void Item::Start() {
    go().GetComponent<Collider>().collisionSignal.connect<&Item::Collision>(this);
}

void Item::Collision(const CollisionEvent& e) {
    if (e.type == CollisionEvent::BEGIN && e.collider.gameobject().name == "PlayerHand") {
        go().transform->Parent(e.collider.gameobject().transform);
    }
}

constexpr Vec<int> keySpriteSize(16, 6);

Key::Key(Fastboi::GORef&& go, KeyColor color) : go(std::move(go)), color(color) { }

void Key::Start() {
    if (!Reqs::HasRequiredComponents(go())) {
        Application::ThrowRuntimeException("Key missing reqs", Application::REQUIREMENTS_NOT_FULFILLED, Reqs::GetMissingNamesString(go()).c_str());
    }

    go().AddComponent<Item>();

    SpriteRenderer& sr = go().GetComponent<SpriteRenderer>();
    sr.cutout = Rect(0, color * keySpriteSize.y, keySpriteSize.x, keySpriteSize.y);
}

void Adventure::KeyGO(Gameobject& go, const Position& pos, Key::KeyColor color) {
    switch (color) {
        case Key::GOLD:
            go.name = "Gold key";
            break;
        case Key::BLACK:
            go.name = "Black key";
            break;
        case Key::WHITE:
            go.name = "White key";
            break;
    }

    constexpr Size keySize(Room::GetTileSize().x, Room::GetTileSize().y * (float) keySpriteSize.y / (float) keySpriteSize.x);

    go.AddComponent<Transform>(pos, keySize, 0);
    go.AddComponent<Collider>(Collider::TRIGGER);
    go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::OBJECTS_OVER, 0), "Keys", Rect(0, 0, keySpriteSize.x, keySpriteSize.y));
    go.AddComponent<Key>(color);
}
