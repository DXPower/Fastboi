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
    Rendering::SetColor(50, 255, 0, 255);
    Rendering::Render_Rect<Rendering::FILLED>(gameobject.transform);
}