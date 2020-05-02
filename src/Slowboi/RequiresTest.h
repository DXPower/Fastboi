#pragma once

#include "Fastboi.h"
#include "FastboiComps.h"
#include "ComponentReqs.h"

using namespace Fastboi;

void RequiresTest(Gameobject& go);

struct RedShifter final {
    using Reqs = RequiredComponents<Fastboi::Components::ColorComp>;

    GORef go;
    Fastboi::Components::ColorComp* color;

    RedShifter(GORef&& go);

    void Start();
    void Update();
};

struct SizeOnRed final {
    using Reqs = RequiredComponents<Transform, Fastboi::Components::ColorComp>; 

    const GORef go;
    const Fastboi::Components::ColorComp* color;

    SizeOnRed(GORef&& go);

    void Start();
    void Update();
};