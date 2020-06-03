## The first version of the Fastboi game engine is complete with version 0.1.0 "Sea Turtle"!

Fastboi is a small and lightweight **Entity-Component** engine. `Gameobject`'s can store components, through which `Start()` and `Update()` functions are called.

*Note: tests and documentation are currently incomplete.*

The current features are:
- **Compile-time components with no inheritance!** `Start()` and `Update()` functions are automatically detected and called.
- **Simple rendering**: There are several rendering functions provided out of the box, as well as several `Renderer` components such as `SpriteRenderer`, `WireframeRenderer`, and more.
- **Easy convex collision detection**: You can add any `Shape` class to your `Transform`, define your convex vertices, and Fastboi will automatically throw it into the collision calculation soup.
- **Type safe collisions layers**: You can mask how objects collide using the `CollisionMask` and it's type safe `CollisionLayer` enum. Many of these calculates are resolved at compile time!
- **Worry-free duplication**: Smart Gameobject references `GORef` allow duplication via the component's copy constructor, and the GORef will automatically be updated!
- **Type safe degrees/radians**: With the magic of C++20, you can now do `3.14_rad + 50_deg` and not worry about conversion at all.
- **Easy key/mouse listeners**: There are multiple listeners provided that allow you to have RAII-guarded listening capabilities for player input.
- ***Nano* Signals and Slots**: A very lightweight and easy to use signals and slots implementation is included and core to Fastboi.
- ***SoLoud* Sound**: A fire-and-forget sound library extended with a `Speaker` component that provides multiple panning functionalities to move sound all over the screen.

Included in the box is a remake of the first level of Atari *Adventure*.

Fastboi consists of multiple core components:
* **Transform** to store information about position, rotation, size, and vertices
* **Rigidbody** to store information about positional and rotational velocity, acceleration, and drag
* **Collider** to receive collision events, mask layers, and dispatch events to listeners
* **Renderer** an abstract class that allows you to create custom renderers
* **Speaker** for easy access to sound for Gameobjects, and provides several panning functionalities.

Some example code on creating a Gameobject via an instantiation function.

```cpp
// A component to be added to Bullet
struct BulletHit {
     GORef go; // Stores a reference to the parent Gameobject
     ColorComp* color{};
     int damage;

     // The first argument of GORef&& go is optional. This is automatically detected.
     BulletHit(GORef&& go, int damage) : go(std::move(go)), damage(damage) { }

     // Called at the beginning of the first tick BulletHit is active
     void Start() {
          color = &go().GetComponent<ColorComp>();
          *color = startingColor;

          // Set our Hit function to listen for collisions
          go().GetComponent<Collider>().collisionSignal.connect<&BulletHit::Hit>(this);
     }

     void Update() {
          // Shift the color every tick
          color.r += 5;
          color.g += 2;
          color.b -= 3;
     }

     // Called when this gameobject collides with something
     void Hit(CollisionEvent& e) {
          if (e.type == CollisionEvent::BEGIN && e.HasComponent<Player>()) {
               e.GetComponent<Player>().Damage(this->damage);

               Destroy(go());
          }
     }
}

void SuperBullet(Gameobject& go, const Position& pos, const Vecf& dir) {
     constexpr float velocity = 500.f;

     go.AddComponent<Transform>(pos, Size(100.f, 50.f), 0_deg);
     go.AddComponent<ColorComp>(200, 10, 50, 255);
     go.AddComponent<WireframeRenderer>(RenderData(RenderOrder::PARTICLES));

     go.AddComponent<BulletHit>(20); // Add our custom BulletHit component with a damage of 20

     // Set the bullet's collider to to be a part of the Particles collision layer,
     // and to only collide with colliders in the Player layer
     go.AddComponent<Collider>(CollisionLayer::PARTICLES).Include(CollisionLayer::Player);
}

// And to create an instance of SuperBullet
Gameobject& bullet = Instantiate<SuperBullet>(Position(500.f, 600.f), Vecf(2,1).normalized());
```

However, many premade components are provided for ease of use and development. Provided components:
* BoxColorRenderer
* SpriteRenderer
* RepeatRenderer
* WireframeRenderer
* RectUI, UITexture
* ColorComp
* Spritesheet
* ChangeObserver

Also in the repository (For now), is a test game called "Slowboi." This is merely to demonstrate some of the basic capabilities.
It is rather rudimentary and has no actual gameplay, but that will be added to over time.

#Required libraries/DLLs:
SDL: (SDL2main, SDL2, SDL2_image)..lib in lib/ (.a for Linux/Mac builds)
     (SDL2, SDL2_image, libjpeg-9, libpng16-16, libtiff-5, libwepb-7, zlib1).dll when running the final executable

The following dependencies have been incorporated into the project:
CacheLineSize (for allocator optimizations) https://github.com/NickStrupat/CacheLineSize (header/src file)
Catch2 (Testing suite) https://github.com/catchorg/Catch2 (header files)
cute_c2 (GJK/EPA collision implementation) https://github.com/RandyGaul/cute_headers (header file)
Nano (Slots/Signals implementation) https://github.com/NoAvailableAlias/nano-signal-slot (header file)
SDL2 (Window management, rendering, input) https://github.com/lohedges/aabbcc (dynamic linking)
SoLoud (Sound) https://sol.gfxile.net/soloud/ (static linking)