#include "BoxColorRenderer.h"
#include "Gameobject.h"
#include "ColorComp.h"
#include "Rendering.h"

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
    Fastboi::Rendering::SetColor(color->r, color->g, color->b, color->a);
    Fastboi::Rendering::Render_Rect<Fastboi::Rendering::FILLED>(gameobject.transform);
}