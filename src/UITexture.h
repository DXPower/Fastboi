#include "Fastboi.h"

namespace Fastboi {
    namespace Components {
        struct UITexture final : public Renderer {
            const Texture& texture;

            UITexture(Gameobject& go, const char* textureName);

            void Render() override;
        };
    }
}