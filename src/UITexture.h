#include "Fastboi.h"

namespace Fastboi {
    namespace Components {
        struct UITexture final : public Renderer {
            const Texture& texture;

            UITexture(GORef&& go, const char* textureName);

            void Render() override;
        };
    }
}