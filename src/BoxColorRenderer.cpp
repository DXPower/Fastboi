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

    if (!Reqs::HasRequiredComponents(gameobject)) {
        Application::ThrowRuntimeException("BoxColorRenderer missing components!", Application::REQUIREMENTS_NOT_FULFILLED);
    }

    color = &gameobject.GetComponent<ColorComp>();
}

void BoxColorRenderer::Render() {
    Rendering::SetColor(color->r, color->g, color->b, color->a);
    Rendering::Render_Rect<Rendering::FILLED>(gameobject.transform);
}