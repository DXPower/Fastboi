#include "Renderer.h"
#include "FastboiCore.h"
#include "Gameobject.h"

Renderer::Renderer(Gameobject& go) : Renderer(go, { RenderOrder::GROUND, 0 }) { }

Renderer::Renderer(Gameobject& go, RenderData data) : gameobject(go), data(data) {
    Fastboi::RegisterRenderer(this);
}

Renderer::~Renderer() {
    Fastboi::UnregisterRenderer(this);
}

RenderOrder Renderer::GetOrder() const {
    return data.order;
}

void Renderer::SetOrder(RenderOrder order) {
    Fastboi::ChangeRenderOrder(this, data.order, order); // Register change in render order
    data.order = order;
}

int Renderer::GetZ() const{
    return data.zindex;
}

void Renderer::SetZ(int z) {
    data.zindex = z;
}