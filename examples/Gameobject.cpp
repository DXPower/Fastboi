#include "FastboiCore.h"
#include <C:\Users\myaka\Documents\GitHub\Fastboi\src\Fastboi.h>
#include <C:\Users\myaka\Documents\GitHub\Fastboi\src\Fastboi.h>
#include <string>

using namespace Fastboi;
using namespace Fastboi::Components;

"Example AddComponent"
Gameobject gameobject;

Transform& transform = gameobject.AddComponent<Transform>(Position(25, 35), Size(70, 80));
transform.rotation += 50_deg;

gameobject.AddComponent<ColorComp>(0, 0, 255, 255);

struct Printer {
    GORef go;
    std::string s;

    Printer(GORef&& go, std::string s) : go(std::move(go)), s(s) { }

    void Start() {
        printf("Starting %s with message '%s'\n", go().name.c_str(), s.c_str());
    }

    void Update() {
        printf("Updating %s with message '%s'\n", go().name.c_str(), s.c_str());
    }
};

gameobject.AddComponent<Printer>("A custom component");

"Example GetComponent"
Gameobject go;

go.AddComponent<ColorComp>(100, 100, 0, 255);
ColorComp& color = go.GetComponent<ColorComp>();


"Example Gameobject"
// The instantiation function for a bullet
void BulletInst(Gameobject& go, const Position& pos, const Vecf& dir) {
    go.AddComponent<Transform>(pos, Size(100.f, 50.f), 0_deg); // Add a transform at the given position, with Size 100, 50 and a rotation of 0 degrees
    go.AddComponent<Rigidbody>(250.f * dir.normalized(), 0_deg); // Give the gameobject a velocity of 250 in the given direction, and 0 rotational velocity
    
    // Add a collider with a trigger flag of the mask layer PARTICLES. 
    // Then, only allow it to collide with colliders in the PLAYER or UNITS mask layers.
    go.AddComponent<Collider>(Collider::TRIGGER, CollisionLayer::PARTICLES).mask.Include(CollisionLayer::PLAYER, CollisionLayer::UNITS);

    go.AddComponent<WireframeRenderer>(RenderData(RenderOrder::PARTICLES)); // Add a wireframe renderer in the PARTICLES layer
    go.AddComponent<ColorComp>(255, 0, 0, 255); // Add the color red
}

// Instantiate a Gameobject following the steps in BulletInst
Gameobject& bullet = Fastboi::Instantiate<BulletInst>(Position(500, 350), Vecf(4.f, 1.5f));

// And to destroy the bullet, simply do:
Fastboi::Destroy(bullet);