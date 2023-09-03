#pragma once

#include "Fastboi.h"
#include "ChangeObserver.h"

namespace Fastboi {
    namespace Components {
        struct RepeatRenderer : Fastboi::Renderer {
            private:
            Size lastSize;

            public:
            CTexturePtr baseTexture;
            Texture repeatTexture;
            Size tileSize;
            
            RepeatRenderer(GORef&& gameobject, RenderData data, const char* textureName, const Size& tileSize);
            ~RepeatRenderer();

            void Start() override;
            void Render() override;

            protected:
            // RepeatRenderer(const RepeatRenderer& copy) = default;
            // Renderer& Duplicate() const override;

            private:
            void CreateRepeatTexture(const Size& newSize);
            void WindowSizeChanged(const Fastboi::WindowResizeEvent& e);
        };
    };
};