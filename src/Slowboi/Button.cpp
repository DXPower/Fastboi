#include "Button.h"
#include "UITexture.h"

using namespace Slowboi;

Button::Button(const Position& p, const Size& s) : Gameobject() {
    Transform& t = AddComponent<Transform>(p, s, 0);
    Components::UITexture& r = AddComponent<Components::UITexture>(*this, "Button");

    clickListener.signal.connect<&Button::Click>(this);
    clickListener.Init(&t, &r);
}

void Button::Click(const TargetedClickEvent& e) const {
    if (e.type == ClickEvent::DOWN) {
        transform->size -= 5.f;
    } else if (e.type == ClickEvent::UP) {
        transform->size += 5.f;
    }
}