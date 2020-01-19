#pragma once

#include "Fastboi.h"

namespace Fastboi {
    namespace Components {
        struct SpriteRenderer final : public Fastboi::Renderer {
            Texture* texture;
            Rect cutout;

            /**
             * @param gameobject Pointer to gameobject owning this renderer
             * @param textureName Key used by Fastboi::Resources to load texture
             * @param cutout Rectangle of texture to render
            **/
            SpriteRenderer(Fastboi::Gameobject& gameobject, Fastboi::RenderData data, const char* textureName, const Rect& cutout);
            ~SpriteRenderer();

            void Render() override;
        };
    };
};