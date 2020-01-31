#pragma once

#include "Fastboi.h"
#include "FastboiComps.h"
#include "ComponentReqs.h"

using namespace Fastboi;

struct RequiresTest final : public Gameobject {
    using Reqs = RequiredComponents<Fastboi::Components::BoxColorRenderer, Transform>;

    RequiresTest();
};

struct RedShifter final {
    using Reqs = RequiredComponents<Fastboi::Components::ColorComp>;

    const Gameobject& go;
    Fastboi::Components::ColorComp* color;

    RedShifter(Gameobject& go);

    void Start();
    void Update();
};