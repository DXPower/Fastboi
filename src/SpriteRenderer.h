#pragma once

#include "Fastboi.h"

namespace Fastboi {
    namespace Components {
        struct SpriteRenderer : public Fastboi::Renderer {
            const Texture& texture;
            Rect cutout;

            /**
             * @param gameobject Pointer to gameobject owning this renderer
             * @param textureName Key used by Fastboi::Resources to load texture
             * @param cutout Rectangle of texture to render
            **/
            SpriteRenderer(GORef&& gameobject, Fastboi::RenderData data, const char* textureName, const Rect& cutout);
            SpriteRenderer(GORef&& gameobject, Fastboi::RenderData data, const Texture& texture, const Rect& cutout);
            ~SpriteRenderer();

            void Render() override;
        };
    };
};