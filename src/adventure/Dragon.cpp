#include "Dragon.h"
#include "FastboiComps.h"
#include "Item.h"
#include "Player.h"
#include "Room.h"

using namespace Adventure;

constexpr Vec<int> dragonSpriteSize(16, 44);

Dragon::Dragon(GORef&& go, Gameobject& player, const DragonStats& stats) : go(std::move(go)), player(player), stats(stats) { }

void Dragon::Start() {
    Reqs::StandardThrowingCheck(go());

    rb = &go().GetComponent<Rigidbody>();
}

void Dragon::Update() {
    if (!doChase || IsDead()) return;

    if (!isBiting) {
        // Chase player
        Vecf dir = GetBiteLocation() - go().transform->position;
        rb->velocity = dir.normalized() * stats.speed;
    } else {
        rb->velocity = Velocity::zero();

        // Wait for 2s to pass before closing jaws
        if (biteTimer.TimeSinceLastTick() >= std::chrono::duration<int>(2)) {
            CloseJaws();
        }
    }
}

void Dragon::CloseJaws() {
    jaws->collider->SetFlags(Collider::TRIGGER);
    go().GetComponent<SpriteRenderer>().cutout.x = 0;
    isBiting = false;

    const auto curColls = jaws->collider->GetCurrentCollisions();

    for (const Collider* c : curColls) {
        if (c->gameobject().name == "Player") { // Search for player inside the jaw
            // Move player to belly
            Transform& plTr = *player.transform;
            player.transform->position = belly->transform->position - plTr.size * Size(0, 0.75f);
            player.GetComponent<Player>().Eat(); // Disables player's movement and collider

            // Stop us from moving
            doChase = false;
        }
    }
    
    // Disable the mouth pieces since the jaw is now closed
    for (Gameobject* o : mouthPieces) {
        o->Disable();
    }
}

void Dragon::Bite() {
    // Set to the bite sprite and enable the mouth pieces to keep us from moving out
    go().GetComponent<SpriteRenderer>().cutout.x = dragonSpriteSize.x;

    for (Gameobject* o : mouthPieces) {
        o->Enable();
    }

    jaws->collider->SetFlags(Collider::TRIGGER); // The jaw doesn't collide anymore since the mouth is open
    rb->velocity = Velocity::zero();

    go().transform->position = GetBiteLocation(); // Teleport to the jaw

    // Start the bite timer
    biteTimer.Tick();
    isBiting = true;    
}

Position Dragon::GetBiteLocation() const {
    const Position curJawPos = jaws->transform->position - player.transform->size * Size(0.75f, 0);
    return go().transform->position + player.transform->position - curJawPos;
}

void Dragon::SwordHit() {
    stats.health--;

    if (stats.health == 0) {
        // "Kill" the dragon
        go().GetComponent<SpriteRenderer>().cutout.x = dragonSpriteSize.x * 2;
        rb->velocity = Velocity::zero();
    }
}

void Dragon::Collision(const CollisionEvent& e) {
    if (isBiting || IsDead()) return;

    if (e.type == CollisionEvent::BEGIN) {
        if (e.collider.gameobject().HasComponent<Sword>()) {
            SwordHit();
        } else if (e.collider.gameobject().name == "Player") {
            Bite();
        }
    }
}

struct DragonShape;

void DragonColPart(Gameobject& go, Dragon& dragon, const Position& pos, const Size& size, double rot) {
    go.AddComponent<Transform>(pos, size, rot).Parent(dragon.go().transform);
    // go.AddComponent<WireframeRenderer>(RenderData(RenderOrder::UI));
    go.AddComponent<ColorComp>(0, 0, 255, 255);

    Collider& coll = go.AddComponent<Collider>(Collider::FIXED, CollisionLayer::UNITS);

    coll.mask.Include(CollisionLayer::PLAYER, CollisionLayer::ITEMS);
    coll.collisionSignal.connect<&Dragon::Collision>(dragon);
}

void Dragon::InstDelegate(Gameobject& go, Gameobject& player, const Position& pos, const DragonStats& stats) {
    constexpr Size dragonSize(1.125f * Room::GetTileSize().x, 1.125f * Room::GetTileSize().x * (float) dragonSpriteSize.y / (float) dragonSpriteSize.x);

    Transform& tr = go.AddComponent<Transform>(pos, dragonSize, 0);

    go.AddComponent<Collider>(Collider::TRIGGER, CollisionLayer::UNITS).mask.Include(CollisionLayer::PLAYER, CollisionLayer::ITEMS);
    go.AddComponent<Rigidbody>();
    
    Dragon& dragon = go.AddComponent<Dragon>(player, stats);

    // Build collider parts
    const Size& size = tr.size;
    const Position& p = tr.position;

    dragon.belly = &Instantiate<DragonColPart>(dragon, p + size * Size(0, .25f), size * Size(1, .5), 0); // Belly + Tail
    dragon.jaws = &Instantiate<DragonColPart>(dragon, p + size * Size(-.125, -.275f), size * Size(1 / 1.35f, 1 / 10.f), 0); // Jaws
    
    dragon.mouthPieces[0] = &Instantiate<DragonColPart>(dragon, p + size * Size(-.125, -.19f), size * Size(1 / 9.5f, 1 / 3.f), 45); // Lower mouth
    dragon.mouthPieces[1] = &Instantiate<DragonColPart>(dragon, p + size * Size(-.125, -.36f), size * Size(1 / 9.5f, 1 / 3.f), -45); // Upper mouth
    
    Instantiate<DragonColPart>(dragon, p + size * Size(.125f, -.05f), size * Size(1 / 1.33f, 1 / 10.f), 0); // Shoulders
    Instantiate<DragonColPart>(dragon, p + size * Size(.18f, -.15f), size * Size(1 / 10.f, 1 / 3.f), 0); // Neck
    Instantiate<DragonColPart>(dragon, p + size * Size(.18f, -.28f), size * Size(1 / 2.5f, 1 / 4.2f), 30); // Head

    for (Gameobject* o : dragon.mouthPieces) {
        o->Disable();
    }
}

Dragon::Jaws::Jaws(GORef&& go) : go(std::move(go)) { };

Yorgle::Yorgle() : DragonStats(1, Player::speed / 1.5f) { };

void Yorgle::Inst(Gameobject& go, Gameobject& player, const Position& pos) {
    Dragon::InstDelegate(go, player, pos, Yorgle());

    go.AddComponent<ColorComp>(0, 0, 255, 255);
    // go.AddComponent<WireframeRenderer>(RenderData(RenderOrder::UNITS, 0));
    go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::UNITS, 20), "Yorgle", Rect(0, 0, dragonSpriteSize.x, dragonSpriteSize.y));
}