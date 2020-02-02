#include "RequiresTest.h"

void RequiresTest(Gameobject& go) {
    go.AddComponent<Transform>(Position(75, 75), Size(250, 100), 0);
    go.AddComponent<Components::ColorComp>();
    go.AddComponent<Components::BoxColorRenderer>(go, RenderData(RenderOrder::GROUND));

    go.AddComponent<RedShifter>(go);
    go.AddComponent<SizeOnRed>(go);
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

SizeOnRed::SizeOnRed(Gameobject& go) : go(go) { }

void SizeOnRed::Start() {
    if (!Reqs::HasRequiredComponents(go))
        Application::ThrowRuntimeException("SizeOnRed reqs not met\n", Application::REQUIREMENTS_NOT_FULFILLED);

    color = &go.GetComponent<Components::ColorComp>();

}

void SizeOnRed::Update() {
    go.transform->size.x = color->r;
}