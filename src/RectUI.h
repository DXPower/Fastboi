#pragma once

#include "Renderer.h"
#include "Events.h"
#include "Input.h"
#include <memory>

struct ColorComp;

struct RectUI final : public Renderer {
    private:
    const ColorComp* color;
    Fastboi::Input::TargetedClickListener clickListener;
    
    public:
    RectUI(Gameobject& go);
    RectUI(Gameobject& go, int z);

    void Start() override;
    void Render() override;

    void Click(const Fastboi::TargetedClickEvent& e);
};