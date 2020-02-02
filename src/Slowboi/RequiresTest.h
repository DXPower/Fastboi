#pragma once

#include "Fastboi.h"
#include "FastboiComps.h"
#include "ComponentReqs.h"

using namespace Fastboi;

void RequiresTest(Gameobject& go);

struct RedShifter final {
    using Reqs = RequiredComponents<Fastboi::Components::ColorComp>;

    const Gameobject& go;
    Fastboi::Components::ColorComp* color;

    RedShifter(Gameobject& go);

    void Start();
    void Update();
};

struct SizeOnRed final {
    using Reqs = RequiredComponents<Transform, Fastboi::Components::ColorComp>;

    const Gameobject& go;
    Fastboi::Components::ColorComp* color;

    SizeOnRed(Gameobject& go);

    void Start();
    void Update();
};