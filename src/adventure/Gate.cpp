#include "Gate.h"
#include "Item.h"
#include "Level.h"
#include "Room.h"
#include "FastboiComps.h"


using namespace Adventure;

constexpr Vec<int> gateSpriteSize{14, 32};
constexpr Size gateSize{2 * Room::GetTileSize().y * (float) gateSpriteSize.x / (float) gateSpriteSize.y, 2 * Room::GetTileSize().y};

Gate::Gate(GORef&& go, KeyColor color, const Room& inside) : go(std::move(go)), color(color), inside(inside) { }

void Gate::Start() {
    if (!Reqs::HasRequiredComponents(go())) {
        Application::ThrowRuntimeException("Key missing reqs", Application::REQUIREMENTS_NOT_FULFILLED, Reqs::GetMissingNamesString(go()).c_str());
    }

    renderer = &go().GetComponent<SpriteRenderer>();

    printf("MAX_PROGRESS: %i\n", MAX_PROGRESS);
    SetProgress(MAX_PROGRESS);

    go().GetComponent<Collider>().collisionSignal.connect<&Gate::Collision>(this);

    // Set enter teleport trigger
    Gameobject& trigger = Fastboi::Instantiate<Gameobject>("Gate trigger");
    Transform& tr = *go().transform;
    Size tileSize = Room::GetTileSize();

    trigger.AddComponent<Transform>(
        Position(tr.position.x, tr.GetBounds().lowerBounds.y - tileSize.y / 2.f)
        , tileSize
        , 0
    );

    Collider& col = trigger.AddComponent<Collider>(Collider::TRIGGER, CollisionLayer::TRIGGERS);
    
    col.mask.Include(CollisionLayer::PLAYER);
    col.collisionSignal.connect<&Gate::EnterTriggerHit>(this);

}

void Gate::Update() {
    if (!hasEntered) {
        if (curProgress != MAX_PROGRESS) {
            timeElapsed += Fastboi::tickDelta;
            
            if (unsigned char progress = timeElapsed * (float) MAX_PROGRESS; progress <= 6) {
                SetProgress(progress);

                if (curState == GateState::CLOSED && progress == MAX_PROGRESS)
                    go().collider->SetEnabled(true);
            }
        }
    } else {
        // Check for teleporting out of castle
        if (holder->transform->position.y >= inside.GetCenter().y + inside.GetSize().y / 2.f - inside.GetTileSize().y / 2) {
            // holder->transform->position = go().transform->position;
            holder->GetComponent<RoomObserver>().ForceTeleport(go().transform->position);

            hasEntered = false;
            timeElapsed = 2.f;

            printf("TPed out\n");

            if (key->GetComponent<Item>().isHeld) {
                printf("Key with player!\n");
                curState = GateState::CLOSED;
                timeElapsed = 0.f;
            }
        }
    }
}

void Gate::TryOpen(const Key& key) {
    if (key.color == color && key.go().GetComponent<Item>().isHeld) {
        printf("Has key!\n");
        curState = GateState::OPEN;
        timeElapsed = 0.0f;

        SetProgress(MIN_PROGRESS);
        go().collider->SetEnabled(false);

        this->key = &key.go();
        holder = &key.go().transform->Parent().gameobject();
    }
}

void Gate::TryEnterGate(Gameobject& player) {
    if (curState == GateState::CLOSED) return;

    Position tpPos = inside.GetCenter();
    tpPos.y += Room::GetSize().y / 2.f - Room::GetTileSize().y * 1.5f;

    player.GetComponent<RoomObserver>().ForceTeleport(tpPos);

    hasEntered = true;
    SetProgress(MAX_PROGRESS);
}

void Gate::EnterTriggerHit(const CollisionEvent& e) {
    printf("Collision on trigger!\n");

    Gameobject& colgo = e.collider.gameobject();
    
    if (e.type == CollisionEvent::BEGIN && colgo.name == "Player")
        TryEnterGate(colgo);
}

void Gate::Collision(const CollisionEvent& e) {
    printf("Collision on door!\n");

    Gameobject& colgo = e.collider.gameobject();
    
    if (e.type == CollisionEvent::BEGIN && colgo.HasComponent<Key>())
        TryOpen(colgo.GetComponent<Key>());
}

void Gate::SetProgress(unsigned char progress) {
    curProgress = progress;
    auto lastH = renderer->cutout.h;

    switch (curState) {
        case GateState::CLOSED:
            renderer->cutout.h = 8 + 4 * progress;
            break;
        case GateState::OPEN:
            renderer->cutout.h = gateSpriteSize.y - 4 * progress;
            break;
    }

    // Check if height changed
    if (lastH != renderer->cutout.h) {
        Transform& tr = *go().transform;
        Size oldSize = tr.size;
        
        float percY = 1.f - (float) renderer->cutout.h / (float) gateSpriteSize.y;
        tr.size.y = gateSize.y - (gateSize.y * percY);

        tr.position.y -= (oldSize.y - tr.size.y) * 0.5f;
    }
}

void Gate::Inst(Gameobject& go, const Position& pos, KeyColor color, const Room& room) {
    go.AddComponent<Transform>(pos, gateSize, 0);
    go.AddComponent<Collider>(Collider::FIXED, CollisionLayer::WALLS).mask.Include(CollisionLayer::PLAYER, CollisionLayer::ITEMS);
    go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::OBJECTS_OVER, 10), "Gate", Rect(0, 0, gateSpriteSize.x, gateSpriteSize.y));
    go.AddComponent<Gate>(color, room);
}

void Castle::Inst(Gameobject& go, const Position& pos, KeyColor color) {
    constexpr Vec<int> castleSpriteSize(160, 145);
    constexpr Size castleSize(10 * Room::GetTileSize().x, 9 * Room::GetTileSize().y);
    go.AddComponent<Transform>(pos, castleSize, 0);
    go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::OBJECTS_UNDER), "Castle", Rect(0, 0, castleSpriteSize.x, castleSpriteSize.y));
}