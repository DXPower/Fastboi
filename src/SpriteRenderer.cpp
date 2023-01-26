#include "SpriteRenderer.h"
#include "Rendering.h"

using namespace Fastboi;
using namespace Fastboi::Components;

SpriteRenderer::SpriteRenderer(GORef&& gameobject, RenderData data, std::string_view textureName, const Rect& cutout)
    : Renderer(std::move(gameobject), data)
    , texture(&Resources::GetTexture(textureName))
    , cutout(cutout) {
}

SpriteRenderer::SpriteRenderer(GORef&& gameobject, RenderData data, CTextureRef texture, const Rect& cutout)
    : Renderer(std::move(gameobject), data)
    , texture(&texture)
    , cutout(cutout) {
}

SpriteRenderer::~SpriteRenderer() { }

void SpriteRenderer::Render() {
    if (screen.has_value()) {
        Rendering::RenderScreen_Texture(screen->RelativizeTransform(*gameobject().transform), *texture, cutout);
    } else {
        Rendering::Render_Texture(gameobject().transform, *texture, cutout);
    }
}

Renderer& SpriteRenderer::Duplicate() const {
    return *(new SpriteRenderer(*this));
}