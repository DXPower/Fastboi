#pragma once

#include "Fastboi.h"
#include "ColorComp.h"

namespace Fastboi {
    namespace Components {
        struct BoxColorRenderer final : public Fastboi::Renderer {
            using Reqs = RequiredComponents<Transform, ColorComp>;

            const ColorComp* color;

            BoxColorRenderer(GORef&& go, RenderData data);
            ~BoxColorRenderer();

            void Start() override;
            void Render() override;
        };
    };
};