#include "SpriteRenderer.h"

using namespace Fastboi;
using namespace Fastboi::Components;

SpriteRenderer::SpriteRenderer(GORef&& gameobject, RenderData data, const char* textureName, const Rect& cutout)
    : Renderer(std::forward<GORef>(gameobject), data)
    , texture(Resources::GetTexture(textureName))
    , cutout(cutout) {
}

SpriteRenderer::SpriteRenderer(GORef&& gameobject, RenderData data, const Texture& texture, const Rect& cutout)
    : Renderer(std::forward<GORef>(gameobject), data)
    , texture(texture)
    , cutout(cutout) {
}

SpriteRenderer::~SpriteRenderer() { }

void SpriteRenderer::Render() {
    Rendering::Render_Texture(gameobject().transform, texture, cutout);
}