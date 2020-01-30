#include "Fastboi.h"
#include "Game.h"
#include "Player.h"
#include "FastboiComps.h"

#include "circular_vector.h"

using namespace Slowboi;
using namespace Fastboi;
using namespace Fastboi::Components;

void LoadResources() {
    Fastboi::Resources::LoadImage("Player", "penguin.png");
    Fastboi::Resources::LoadImage("Brick", "brick.png");
}

void Slowboi::InitGame() {
    circular_vector<int> cvec;
    cvec = { 1, 2, 3, 4, 5, 6 };

    for (auto pit = cvec.pair_begin(); pit != cvec.pair_end(); pit++) {
        printf("%i %i\n", *pit.first, *pit.second);
    }

    LoadResources();

    using namespace Fastboi;

    PlayerActor& player = Instantiate<PlayerActor>(Position(500.f, 500.f));
    Brick& brick = Instantiate<Brick>(Position(700.f, 700.f));

    Instantiate<UISquare>(Position(00, 0), Size(50, 50), ColorComp(0, 0, 255, 255), 2).name = "1";
    Instantiate<UISquare>(Position(50, 0), Size(50, 50), ColorComp(0, 255, 0, 255), 1).name = "2";
    Instantiate<UISquare>(Position(100, 0), Size(50, 50), ColorComp(255, 0, 0, 255), 0).name = "3";

    Instantiate<UISquare>(Position(200, 200), Size(150, 150), ColorComp(255, 0, 0, 255), 0).name = "4";
    Instantiate<UISquare>(Position(200, 200), Size(50, 50), ColorComp(0, 255, 0, 255), 4).name = "5";

    printf("Game initiated\n");

    SetCamera(Camera(*player.transform, Camera::WATCHING, 1.5f));
}

Bullet::Bullet(const Position& p, const Velocity& v) : Gameobject("Bullet") {
    AddComponent<Transform>();
    transform->position = p;
    transform->size = Size(30.f, 10.f);

    AddComponent<VelocityComponent>(v);

    printf("Making bullet with position: %f %f, velocity %f %f\n", p.x, p.y, v.x, v.y);

    AddComponent<ColorComp>();
    GetComponent<ColorComp>().set(0, 0, 255, 255);

    AddComponent<ColorShiftComp>(this);

    AddComponent<BoxColorRenderer>(*this, RenderData(RenderOrder::PARTICLES));
    Collider& collider = AddComponent<Collider>(*this, Collider::TRIGGER);
    collider.collisionSignal.connect<&Bullet::Hit>(this);

    printf("New bullet actor!\n");
}

void Bullet::Hit(const Fastboi::CollisionEvent& e) {
    if (e.collider.IsTrigger() || e.collider.gameobject.HasComponent<Components::Player>()) return;

    Fastboi::camera.SetTarget(*(new Transform(*transform)), Camera::OWNING);

    Fastboi::Destroy(*this);
}

PlayerActor::PlayerActor(const Position& p) : Gameobject("Player") {
    printf("Creating player\n");
    AddComponent<Transform>();
    printf("Transform added\n");
    transform->position = p;
    transform->size = Size(41.f, 42.f);

    printf("Making collider...\n");
    Collider& collider = AddComponent<Collider>(*this);
    printf("Collider made\n");

    AddComponent<ColorComp>();
    GetComponent<ColorComp>().set(255, 100, 0, 255);

    AddComponent<SpriteRenderer>(*this, RenderData(RenderOrder::UNITS), "Player", Rect(0, 0, 41, 42));

    using PlayerSpritesheet = Spritesheet<>;
    using Animation = PlayerSpritesheet::Animation;

    printf("Adding spritesheet...");
    PlayerSpritesheet& spritesheet = AddComponent<PlayerSpritesheet>(this);
    printf("Adding animations...");

    for (uint8_t i = 0; i < 8; i++) {
        constexpr uint32_t ticksWalking = 1;
        constexpr uint32_t ticksStanding = 40;
        constexpr uint32_t height = 42;

        spritesheet.AddAnimation(i + 1, Animation(Veci(0, i * 42), Vec<int>(41, 42), Vecb(8, 1), ticksWalking));
        spritesheet.AddAnimation(-i - 1, Animation(Veci(0, i * 42), Vec<int>(41, 42), Vecb(4, 1), ticksStanding));
    }
    printf("Added\n");

    spritesheet.SetCurrentAnimation(-1);
    printf("Current animation set\n");
    printf("Adding player\n");
    Slowboi::Components::Player& player = AddComponent<Slowboi::Components::Player>(this);

    printf("New player actor!\n");
}

Brick::Brick(const Position& position)
 : Gameobject("Brick")
 , expandListener(Input::KeyListener(SDL_SCANCODE_SPACE)) {
    printf("Creating brick...\n");
    AddComponent<Transform>();
    transform->position = position;
    transform->size = Size(200.f, 200.f);
    printf("Transform made.\n");

    AddComponent<RepeatRenderer>(*this, RenderData(RenderOrder::GROUND), "Brick", Size(80.f, 80.f));
    AddComponent<Collider>(*this, Collider::FIXED);

    expandListener.signal->connect<&Brick::Expand>(this);

    // struct Recreater {
    //     Gameobject& go;
    //     Recreater(Gameobject& go) : go(go) { };

    //     int timer = 0;

    //     void Update() {
    //         if (timer++ == 100) {
    //             RepeatRenderer& rr = go.GetComponent<RepeatRenderer>();
    //             printf("RR texture ptr before: %p\n", rr.repeatTexture.GetSDL_Texture());
    //             // rr.repeatTexture.Recreate(go.transform->size, SDL_TEXTUREACCESS_TARGET, rr.repeatTexture.GetFormat());
    //             rr.CreateRepeatTexture(rr.gameobject.transform->size);
    //             printf("RR texture ptr after: %p\n", rr.repeatTexture.GetSDL_Texture());
    //             timer = 0;
    //         }
    //     }
    // };

    // AddComponent<Recreater>(*this);

    printf("Brick made\n");
}

void Brick::Expand(const Fastboi::KeyEvent& e) {
    printf("Space pressed! Expanding and rotating...\n");
    transform->size += 5.f;
    transform->SetRotation(transform->rotation + 10.f);

    if (transform->size.x >=800.f) {
        delete this;
    }
}

UISquare::UISquare(const Position& p, const Size& s, const ColorComp& color, int zindex) {
    AddComponent<Transform>(p, s, 0);
    AddComponent<ColorComp>(color.r, color.g, color.b, color.a);
    AddComponent<RectUI>(*this, zindex);
}
