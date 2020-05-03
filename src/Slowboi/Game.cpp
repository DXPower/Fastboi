#include "Fastboi.h"
#include "Game.h"
#include "Player.h"
#include "Button.h"
#include "GroundBG.h"
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
    Fastboi::Resources::LoadImage("Spritemap", "spritemap.png");
}

struct Spinner;

void Slowboi::InitGame() {
    LoadResources();

    // Instantiate<Slowboi::Bullet>(Position(500, 500), Size(526, 53));

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
    anon.AddComponent<BoxColorRenderer>(RenderData(RenderOrder::GROUND, 10));
    anon.AddComponent<Collider>(Collider::FIXED);

    std::vector<Rect> grassSprites = { Rect(36, 36, 8, 8) };
    Gameobject& grass = Instantiate<&Slowboi::GroundBG>(Position(200, 200), Size(500, 500), Resources::GetTexture("Spritemap"), Size(50, 50), grassSprites);

    SetCamera(Camera(*player.transform, Camera::WATCHING, 1.5f));

    pauseListener.signal.connect<&TogglePause>();

    Gameobject& moveLeft = Instantiate<Gameobject>("Left mover");
    moveLeft.AddComponent<Transform>(Position(380, 550), Size(20, 20), 0);
    moveLeft.AddComponent<Rigidbody>(Velocity::zero(), 300).rotDrag = 70.;
    moveLeft.AddComponent<ColorComp>(255, 0, 0, 255);
    moveLeft.AddComponent<WireframeRenderer>(RenderData(RenderOrder::PARTICLES));

    //! Now let's add something to move down
    Gameobject& moveDown = Instantiate<Gameobject>("Down mover");
    moveDown.AddComponent<Transform>(Position(500, 550), Size(100, 20), 0);
    moveDown.AddComponent<Rigidbody>(Velocity(0, 50.f), 80);
    moveDown.AddComponent<ColorComp>(255, 168, 100, 255);
    moveDown.AddComponent<WireframeRenderer>(RenderData(RenderOrder::PARTICLES));

    //! Now let's make the left mover a child of the down mover
    moveLeft.transform->Parent(moveDown.transform);

    //! Now I want to make something move to the right as a child of the left-mover to cancel out its movement
    Gameobject& moveRight = Instantiate<Gameobject>("Right mover");
    moveRight.AddComponent<Transform>(Position(380, 500), Size(20, 20), 0);
    moveRight.AddComponent<Rigidbody>(Velocity::zero(), 500);
    moveRight.AddComponent<ColorComp>(100, 168, 255, 255);
    moveRight.AddComponent<WireframeRenderer>(RenderData(RenderOrder::PARTICLES, 1));
    moveRight.AddComponent<Collider>(Collider::FIXED);

    // //! And now let's make right mover a child of left mover
    moveRight.transform->Parent(moveLeft.transform);
}

struct BulletHit {
    GORef go;
    Slowboi::Components::Player& player;

    BulletHit(GORef&& go, Slowboi::Components::Player& player) : go(std::move(go)), player(player) { };
    
    void Start() {
        go().GetComponent<Collider>().collisionSignal.connect<&BulletHit::Hit>(this);
    }

    void Hit(const CollisionEvent& e) {
        if (e.type == CollisionEvent::END) return;

        if (!e.collider.IsTrigger() && !e.collider.gameobject().HasComponent<Slowboi::Components::Player>()) {
            player.gameobject().GetComponent<Rigidbody>().rotVelocity = go().GetComponent<Rigidbody>().rotVelocity;
            
            // if (!e.collider.gameobject().HasComponent<Spinner>())
                // go().DuplicateComponent<Spinner>(e.collider.gameobject()).speed = -0.1;

            // e.collider.gameobject().transform->Parent(player.gameobject().transform.get());

            // Gameobject& newBullet = go().Duplicate();
            // newBullet.transform->position = player.gameobject().transform->position;

            // Gameobject& newWall = e.collider.gameobject().Duplicate();
            // newWall.transform->position = newWall.transform->position += 150;

            Fastboi::Destroy(go());
        }
    }
};

static int bulletCount = 0;

void Slowboi::Bullet(Gameobject& go, const Position& p, const Velocity& v, Components::Player& player) {
    // char* buffer = new char[20];
    // sprintf(buffer, "Bullet %i", bulletCount);
    // bulletCount++;

    go.name = "Bullet";

    go.AddComponent<Transform>(p, Size(30, 10), 0);
    go.AddComponent<Rigidbody>(v, 500).rotDrag = 300.;

    go.AddComponent<ColorComp>(0, 0, 255, 255);
    go.AddComponent<ColorShiftComp>();

    go.AddComponent<WireframeRenderer>(RenderData(RenderOrder::PARTICLES));

    BulletHit& bh = go.AddComponent<BulletHit>(player);
    Collider& coll = go.AddComponent<Collider>(Collider::TRIGGER);
}

void Slowboi::PlayerGO(Gameobject& go, const Position& p) {
    go.name = "Player";

    go.AddComponent<Transform>(p, Size(41, 42), 0);
    go.AddComponent<Collider>();

    go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::UNITS), "Player", Rect(0, 0, 41, 42));
    Slowboi::Components::Player& player = go.AddComponent<Slowboi::Components::Player>();
}

struct Expander {
    GORef go;
    Input::KeyListener expandListener = Input::KeyListener(SDL_SCANCODE_SPACE);

    Expander(GORef&& go) : go(std::move(go)) {
        expandListener.signal.connect<&Expander::Expand>(this);
    };

    ~Expander() {
        printf("Expander component destroyed!\n");
    }

    void Expand(const KeyEvent& e) {
        if (e.type == KeyEvent::DOWN) {
            go().transform->size += 5.f;
            go().transform->SetRotation(go().transform->rotation + 10.f);
        }
    }
};

void Slowboi::Brick(Gameobject& go, const Position& p) {
    go.name = "Brick";

    go.AddComponent<Transform>(p, Size(200, 200), 0);
    
    go.AddComponent<RepeatRenderer>(RenderData(RenderOrder::GROUND), "Brick", Size(80, 80));
    go.AddComponent<Collider>(Collider::FIXED);

    go.AddComponent<Expander>();
}

void Slowboi::UISquare(Gameobject& go, const Position& p, const Size& s, const ColorComp& color, int zindex) {
    go.AddComponent<Transform>(p, s, 0);
    go.AddComponent<ColorComp>(color.r, color.g, color.b, color.a);
    go.AddComponent<RectUI>(zindex);
}
