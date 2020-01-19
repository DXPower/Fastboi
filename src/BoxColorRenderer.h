#pragma once

#include "Fastboi.h"
#include "ColorComp.h"

namespace Fastboi {
    namespace Components {
        struct BoxColorRenderer final : public Fastboi::Renderer {
            const ColorComp* color;

            BoxColorRenderer(Fastboi::Gameobject& go, RenderData data);
            ~BoxColorRenderer();

            void Start() override;
            void Render() override;
        };
    };
};