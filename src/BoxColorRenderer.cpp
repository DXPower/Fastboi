#include "BoxColorRenderer.h"

using namespace Fastboi;
using namespace Fastboi::Components;

BoxColorRenderer::BoxColorRenderer(GORef&& go, RenderData data) : Renderer(std::move(go), data) { }

BoxColorRenderer::~BoxColorRenderer() {

}

void BoxColorRenderer::Start() {
    Renderer::Start();

    if (!Reqs::HasRequiredComponents(gameobject())) {
        Application::ThrowRuntimeException("BoxColorRenderer missing components!", Application::REQUIREMENTS_NOT_FULFILLED);
    }

    color = &gameobject().GetComponent<ColorComp>();
}

void BoxColorRenderer::Render() {
    printf("Setting color\n");
    Rendering::SetColor(color->r, color->g, color->b, color->a);
    printf("Set color. Senting rect...\n");
    printf("Transform:\n");
    printf("%f %f %f %f\n", gameobject().transform->position.x, gameobject().transform->position.y, gameobject().transform->size.x, gameobject().transform->size.y);
    Rendering::Render_Rect<Rendering::FILLED>(gameobject().transform);
    printf("Rect rendered\n");
}

Renderer& BoxColorRenderer::Duplicate() const {
    return *(new BoxColorRenderer(*this));
}