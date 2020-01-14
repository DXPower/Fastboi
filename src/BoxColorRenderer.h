#pragma once
#include "Renderer.h"
#include "ColorComp.h"

struct BoxColorRenderer final : public Renderer {
    const ColorComp* color;

    BoxColorRenderer();
    BoxColorRenderer(Gameobject* go, RenderData data);
    ~BoxColorRenderer();

    void Start() override;
    void Render() override;
};