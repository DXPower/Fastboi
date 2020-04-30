#include "Renderer.h"
#include "FastboiCore.h"
#include "Gameobject.h"
#include "GORef.h"

using namespace Fastboi;

Renderer::Renderer(GORef&& go) : Renderer(std::move(go), { RenderOrder::GROUND, 0 }) { }

Renderer::Renderer(GORef&& go, RenderData data) : gameobject(std::move(go)), data(data) {
    Fastboi::RegisterRenderer(this);
    printf("Registering renderer at %p\n", this);
}

Renderer::Renderer(const Renderer& copy) : Renderer(GORef(copy.gameobject), copy.data) {} // Nope, I'm dumb, should be copy.data

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