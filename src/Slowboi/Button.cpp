#include "Button.h"
#include "UITexture.h"

using namespace Slowboi;
using namespace Fastboi;
using namespace Input;

struct ButtonClickComp {
    Gameobject& go;

    ButtonClickComp(Gameobject& go) : go(go) { };

    void Click(const ClickEvent& e) {
        if (e.type == ClickEvent::DOWN) {
            go.transform->size -= 5.f;
        } else if (e.type == ClickEvent::UP) {
            go.transform->size += 5.f;
        }
    }
};

void Button(Gameobject& go, const Position& p, const Size& s) {
    Transform& t = go.AddComponent<Transform>(p, s, 0);
    Components::UITexture& r = go.AddComponent<Components::UITexture>(go, "Button");
    ButtonClickComp& bcc = go.AddComponent<ButtonClickComp>(go);

    ClickListener& cl = go.AddComponent<ClickListener>();
    cl.signal.connect<&ButtonClickComp::Click>(&bcc);
}
