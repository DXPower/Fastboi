#pragma once

#include "Fastboi.h"

namespace Fastboi {
    namespace Components {
        struct WireframeRenderer final : public Renderer{
            WireframeRenderer(Gameobject& go, RenderData rd);
            ~WireframeRenderer();

            void Start();
            void Render();
        };
    }
}