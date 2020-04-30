#include "WireframeRenderer.h"
#include "ColorComp.h"

using namespace Fastboi;
using namespace Fastboi::Components;

WireframeRenderer::WireframeRenderer(GORef&& go, RenderData rd) : Renderer(std::move(go), rd) { };
WireframeRenderer::~WireframeRenderer() { };

void WireframeRenderer::Start() {
    Renderer::Start();

    using Reqs = RequiredComponents<Transform, ColorComp>;

    if (!Reqs::HasRequiredComponents(gameobject()))
        Application::ThrowRuntimeException("WireframeRenderer missing reqs!", 
                                        Application::REQUIREMENTS_NOT_FULFILLED,
                                        Reqs::GetMissingNamesString(gameobject()).c_str());
}

void WireframeRenderer::Render() {
    // printf("Rendering wireframe renderer! GO Name: %s\n", gameobject().name.c_str());
    Rendering::SetColor(gameobject().GetComponent<ColorComp>());
    
    auto& vertices = gameobject().transform->GetVertices();

    for (auto pit = vertices.pair_begin(); pit != vertices.pair_end(); pit++) {
        Rendering::Render_Line(*pit.first, *pit.second);
    }
}

Renderer& WireframeRenderer::Duplicate() const {
    return *(new WireframeRenderer(*this));
}