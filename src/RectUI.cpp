#include "RectUI.h"
#include "Gameobject.h"
#include "Rendering.h"
#include "ColorComp.h"
#include "Input.h"

using namespace Fastboi;
using namespace Fastboi::Components;

RectUI::RectUI(GORef&& go) : Renderer(std::move(go), { RenderOrder::UI, 0 }) { }
RectUI::RectUI(GORef&& go, int z) : Renderer(std::move(go), { RenderOrder::UI, z }) { }

void RectUI::Start() {
    Renderer::Start();
    
    color = &gameobject().GetComponent<ColorComp>();

    clickListener.signal.connect<&RectUI::Click>(this);
    clickListener.Init(*gameobject().transform, *this);
}

void RectUI::Render() {
    Rendering::SetColor(color->r, color->g, color->b, color->a);
    Rendering::RenderScreen_Rect<Rendering::FILLED>(gameobject().transform);
}

void RectUI::Click(const Fastboi::TargetedClickEvent& e) {
    if (e.type == Fastboi::ClickEvent::DOWN) {
        printf("Click!: Pos: %f %f\n", gameobject().transform->position.x, gameobject().transform->position.y);
        gameobject().transform->position += 35;
    }
}
