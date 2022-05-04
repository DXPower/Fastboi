#include "BoxColorRenderer.h"
#include "Rendering.h"

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
    Rendering::SetColor(color->r, color->g, color->b, color->a);

    if (screen.has_value()) {
        Rendering::RenderScreen_Rect<Rendering::FILLED>(screen->RelativizeTransform(*gameobject().transform));
    } else {
        Rendering::Render_Rect<Rendering::FILLED>(gameobject().transform);
    }
}

Renderer& BoxColorRenderer::Duplicate() const {
    return *(new BoxColorRenderer(*this));
}