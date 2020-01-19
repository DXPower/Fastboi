#include "RectUI.h"
#include "Gameobject.h"
#include "Rendering.h"
#include "ColorComp.h"
#include "Input.h"

using namespace Fastboi;
using namespace Fastboi::Components;

RectUI::RectUI(Gameobject& go) : Renderer(go, { RenderOrder::UI, 0 }) { }
RectUI::RectUI(Gameobject& go, int z) : Renderer(go, { RenderOrder::UI, z }) { }

void RectUI::Start() {
    Renderer::Start();
    
    color = &gameobject.GetComponent<ColorComp>();

    clickListener.signal->connect<&RectUI::Click>(this);
    clickListener.Init(gameobject.transform.get(), this);
}

void RectUI::Render() {
    Rendering::SetColor(color->r, color->g, color->b, color->a);
    Rendering::RenderScreen_Rect<Rendering::FILLED>(gameobject.transform);
}

void RectUI::Click(const Fastboi::TargetedClickEvent& e) {
    if (e.type == Fastboi::ClickEvent::DOWN) {
        printf("Click!: Pos: %f %f\n", gameobject.transform->position.x, gameobject.transform->position.y);
        gameobject.transform->position += 35;
    }
}
