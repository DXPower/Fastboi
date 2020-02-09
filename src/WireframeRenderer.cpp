#include "WireframeRenderer.h"
#include "ColorComp.h"

using namespace Fastboi;
using namespace Fastboi::Components;

WireframeRenderer::WireframeRenderer(Gameobject& go, RenderData rd) : Renderer(go, rd) {
    printf("Creating wireframe renderer!\n");
};
WireframeRenderer::~WireframeRenderer() {
    printf("Destroying wireframe renderer!\n");
};

void WireframeRenderer::Start() {
    Renderer::Start();

    using Reqs = RequiredComponents<Transform, ColorComp>;

    if (!Reqs::HasRequiredComponents(gameobject))
        Application::ThrowRuntimeException("WireframeRenderer missing reqs!", 
                                        Application::REQUIREMENTS_NOT_FULFILLED,
                                        Reqs::GetMissingNamesString(gameobject).c_str());
}

void WireframeRenderer::Render() {
    Rendering::SetColor(gameobject.GetComponent<ColorComp>());
    
    auto& vertices = gameobject.transform->GetVertices();

    for (auto pit = vertices.pair_begin(); pit != vertices.pair_end(); pit++) {
        Rendering::Render_Line(*pit.first, *pit.second);
    }
}