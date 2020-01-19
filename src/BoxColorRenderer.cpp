#include "BoxColorRenderer.h"

using namespace Fastboi;
using namespace Fastboi::Components;

BoxColorRenderer::BoxColorRenderer(Gameobject& go, RenderData data) : Renderer(go, data) {
    printf("Box color renderer made!\n");
}

BoxColorRenderer::~BoxColorRenderer() {

}

void BoxColorRenderer::Start() {
    Renderer::Start();

    color = &gameobject.GetComponent<ColorComp>();
}

void BoxColorRenderer::Render() {
    Rendering::SetColor(color->r, color->g, color->b, color->a);
    Rendering::Render_Rect<Rendering::FILLED>(gameobject.transform);
}