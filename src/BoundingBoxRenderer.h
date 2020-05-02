#pragma once

#include "Renderer.h"

namespace Fastboi {
    struct BoundingBoxRenderer final : public Renderer {
        BoundingBoxRenderer(GORef&& go, const RenderData& rd) : Renderer(std::move(go), rd) { };

        void Render() override;

        protected:
        BoundingBoxRenderer(const BoundingBoxRenderer& copy) = default;
        BoundingBoxRenderer& Duplicate() const override;
    };
}