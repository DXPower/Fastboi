#include "WireframeRenderer.h"

using namespace Fastboi;
using namespace Fastboi::Components;

WireframeRenderer::WireframeRenderer(Gameobject& go, RenderData rd) : Renderer(go, rd) {
    printf("Creating wireframe renderer!\n");
};
WireframeRenderer::~WireframeRenderer() {
    printf("Destroying wireframe renderer!\n");
};

void WireframeRenderer::Render() {
    Rendering::SetColor(0, 0, 255, 255);
    
    auto& vertices = gameobject.transform->GetVertices();

    for (auto pit = vertices.pair_begin(); pit != vertices.pair_end(); pit++) {
        Rendering::Render_Line(*pit.first, *pit.second);
    }
}