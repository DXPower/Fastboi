#include "Button.h"
#include "UITexture.h"

using namespace Slowboi;
using namespace Fastboi;
using namespace Input;

struct ButtonClickComp {
    Gameobject& go;
    TargetedClickListener listener;

    ButtonClickComp(Gameobject& go) : go(go), listener(*go.transform, *go.renderer) {
        listener.signal.connect<&ButtonClickComp::Click>(this);
    };

    void Click(const TargetedClickEvent& e) {
        if (e.type == ClickEvent::DOWN) {
            go.transform->size -= 5.f;
        } else if (e.type == ClickEvent::UP) {
            go.transform->size += 5.f;
        }
    }
};

void Button(Gameobject& go, const Position& p, const Size& s) {
    go.AddComponent<Transform>(p, s, 0);
    go.AddComponent<Components::UITexture>("Button");
    go.AddComponent<ButtonClickComp>();
}
