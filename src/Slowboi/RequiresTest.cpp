#include "RequiresTest.h"

RequiresTest::RequiresTest() : Gameobject("RequiresTest") {
    AddComponent<Transform>(Position(75, 75), Size(250, 100), 0);
    AddComponent<Components::ColorComp>();
    AddComponent<Components::BoxColorRenderer>(*this, RenderData(RenderOrder::PARTICLES));
    AddComponent<RedShifter>(*this);
}

RedShifter::RedShifter(Gameobject& go) : go(go) {
    Fastboi::Print("RedShifter made!\n");
}

void RedShifter::Start() {
    Fastboi::Print("Checking requirements...\n");

    if (!Reqs::HasRequiredComponents(go))
        Application::ThrowRuntimeException("RedShifter requirements not fulfilled\n", Application::REQUIREMENTS_NOT_FULFILLED);

    color = &go.GetComponent<Components::ColorComp>();
}

void RedShifter::Update() {
    color->r += 1;
}