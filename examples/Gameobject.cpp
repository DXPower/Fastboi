#include "src/Fastboi.h"
#include "src/FastboiComps.h"
using namespace Fastboi;
using namespace Fastboi::Components;

"Example AddComponent"
Gameobject go;

Transform& transform = go.AddComponent<Transform>(Position(25, 35), Size(70, 80));
transform.SetRotation(50);

go.AddComponent<ColorComp>(0, 0, 255, 255).;
go.AddComponent<BoxColorRenderer>(go);

"Example GetComponent"
Gameobject go;

go.AddComponent<ColorComp>(100, 100, 0, 255);
ColorComp& color = go.GetComponent<ColorComp>();


"Example Gameobject"
struct Bullet final : public Gameobject {
    Bullet() {
        AddComponent<Transform>(Position(0, 0), Size(100, 20));
        AddComponent<VelocityComp>(Velocity(50, 0));
        AddComponent<ColorComp>(0, 0, 0, 255);
        AddComponent<BoxColorRenderer>(*this, Fastboi::RenderOrder::PARTICLES);

        Collider& c = AddComponent<Collider>(*this);
        c.collisionSignal.connect<&Bullet::Hit>(this);
    }

    ~Bullet() = default;

    void Hit(const CollisionEvent& e) {
        if (e.collider.IsFixed() && !e.collider.IsTrigger())
            Fastboi::Destroy(*this);
    }
};