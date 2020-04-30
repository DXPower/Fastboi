#pragma once

#include "Fastboi.h"

namespace Fastboi {
    namespace Components {
        struct WireframeRenderer final : public Renderer{
            WireframeRenderer(GORef&& go, RenderData rd);
            ~WireframeRenderer();

            void Start() override;
            void Render() override;
            
            protected:
            WireframeRenderer(const WireframeRenderer& copy) = default;
            Renderer& Duplicate() const override;
        };
    }
}