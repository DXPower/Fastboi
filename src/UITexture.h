#include "Fastboi.h"

namespace Fastboi {
    namespace Components {
        struct UITexture final : public Renderer {
            const Texture& texture;
            Rect cutout;

            UITexture(GORef&& go, const char* textureName);
            UITexture(GORef&& go, const char* textureName, const Rect& cutout);

            void Render() override;

            protected:
            UITexture(const UITexture& copy) = default;
            Renderer& Duplicate() const override;
        };
    }
}