#pragma once


#include <string_view>
#include "Fastboi.h"

namespace Fastboi {
    namespace Components {
        struct SpriteRenderer : public Fastboi::Renderer {
            CTexturePtr texture;
            Rect cutout;

            SpriteRenderer(GORef&& gameobject, RenderData data);
            SpriteRenderer(GORef&& gameobject, RenderData data, std::string_view textureName, const Rect& cutout);
            SpriteRenderer(GORef&& gameobject, RenderData data, CTextureRef texture, const Rect& cutout);
            ~SpriteRenderer();

            void Render() override;

            protected:
            SpriteRenderer(const SpriteRenderer& copy) = default;
            Renderer& Duplicate() const override;
        };
    };
};