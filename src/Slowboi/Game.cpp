#include "Fastboi.h"
#include "Game.h"
#include "Player.h"
#include "Button.h"
#include "FastboiComps.h"
#include "RequiresTest.h"

#include "circular_vector.h"

using namespace Slowboi;
using namespace Fastboi;
using namespace Fastboi::Components;

Fastboi::Input::KeyListener pauseListener(SDL_SCANCODE_ESCAPE);

void TogglePause(const Fastboi::KeyEvent& e) {
    if (e.type == KeyEvent::DOWN) {
        if (Fastboi::IsPaused())
            Fastboi::Unpause();
        else
            Fastboi::Pause();
    }
}

void LoadResources() {
    Fastboi::Resources::LoadImage("Player", "penguin.png");
    Fastboi::Resources::LoadImage("Brick", "brick.png");
    Fastboi::Resources::LoadImage("Button", "button.png");
}

void Slowboi::InitGame() {
    LoadResources();

    Instantiate<Slowboi::Bullet>(Position(500, 500), Size(526, 53));

    Gameobject& player = Instantiate<&PlayerGO>(Position(500.f, 500.f));
    Gameobject& brick = Instantiate<Brick>(Position(900, 800));

    Instantiate<UISquare>(Position(00, 0), Size(50, 50), ColorComp(0, 0, 255, 255), 2);
    Instantiate<UISquare>(Position(50, 0), Size(50, 50), ColorComp(0, 255, 0, 255), 1);
    Instantiate<UISquare>(Position(100, 0), Size(50, 50), ColorComp(255, 0, 0, 255), 0);

    Instantiate<UISquare>(Position(200, 200), Size(150, 150), ColorComp(255, 0, 0, 255), 0);
    Instantiate<UISquare>(Position(200, 200), Size(50, 50), ColorComp(0, 255, 0, 255), 4);

    Gameobject& rt = Instantiate<RequiresTest>();
    rt.transform->position = Position(400, 400);

    Gameobject& anon = Instantiate<Gameobject>("Anonymous GO");
    anon.AddComponent<Transform>(Position(50, 500), Size(75, 90), 45);
    anon.AddComponent<ColorComp>(255, 0, 255, 255);
    anon.AddComponent<BoxColorRenderer>(RenderData(RenderOrder::GROUND));
    anon.AddComponent<Collider>(Collider::FIXED);

    SetCamera(Camera(*player.transform, Camera::WATCHING, 1.5f));

    pauseListener.signal.connect<&TogglePause>();
}

struct BulletHit {
    Gameobject& go;

    BulletHit(Gameobject& go) : go(go) { };
    
    void Hit(const CollisionEvent& e) {
        if (!e.collider.IsTrigger() && !e.collider.gameobject.HasComponent<Slowboi::Components::Player>()) {
            Fastboi::Destroy(go);
        }
    }
};

void Slowboi::Bullet(Gameobject& go, const Position& p, const Velocity& v) {
    go.name = "Bullet";

    go.AddComponent<Transform>(p, Size(30, 10), 0);
    go.AddComponent<VelocityComp>(v);

    go.AddComponent<ColorComp>(0, 0, 255, 255);
    go.AddComponent<ColorShiftComp>();

    go.AddComponent<BoxColorRenderer>(RenderData(RenderOrder::PARTICLES));

    BulletHit& bh = go.AddComponent<BulletHit>();
    Collider& coll = go.AddComponent<Collider>(Collider::TRIGGER);
    coll.collisionSignal.connect<&BulletHit::Hit>(bh);
}

void Slowboi::PlayerGO(Gameobject& go, const Position& p) {
    go.name = "Player";

    go.AddComponent<Transform>(p, Size(41, 42), 0);
    go.AddComponent<Collider>();

    go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::UNITS), "Player", Rect(0, 0, 41, 42));
    Slowboi::Components::Player& player = go.AddComponent<Slowboi::Components::Player>();
}

struct Expander {
    Gameobject& go;
    Input::KeyListener expandListener = Input::KeyListener(SDL_SCANCODE_SPACE);

    Expander(Gameobject& go) : go(go) {
        expandListener.signal.connect<&Expander::Expand>(this);
    };

    ~Expander() {
        printf("Expander component destroyed!\n");
    }

    void Expand(const KeyEvent& e) {
        if (e.type == KeyEvent::DOWN) {
            go.transform->size += 5.f;
            go.transform->SetRotation(go.transform->rotation + 10.f);
        }
    }
};

void Slowboi::Brick(Gameobject& go, const Position& p) {
    go.name = "Brick";

    go.AddComponent<Transform>(p, Size(200, 200), 0);
    
    // go.AddComponent<RepeatRenderer>(go, RenderData(RenderOrder::GROUND), "Brick", Size(80, 80));
    go.AddComponent<WireframeRenderer>(RenderData(RenderOrder::UI));
    go.AddComponent<Collider>(Collider::FIXED);

    go.AddComponent<Expander>();
}

void Slowboi::UISquare(Gameobject& go, const Position& p, const Size& s, const ColorComp& color, int zindex) {
    go.AddComponent<Transform>(p, s, 0);
    go.AddComponent<ColorComp>(color.r, color.g, color.b, color.a);
    go.AddComponent<RectUI>(zindex);
}
