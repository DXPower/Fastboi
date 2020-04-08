#include "SpriteRenderer.h"

using namespace Fastboi;
using namespace Fastboi::Components;

SpriteRenderer::SpriteRenderer(Gameobject& gameobject, RenderData data, const char* textureName, const Rect& cutout)
    : Renderer(gameobject, data)
    , texture(Resources::GetTexture(textureName))
    , cutout(cutout) {
}

SpriteRenderer::~SpriteRenderer() { }

void SpriteRenderer::Render() {
    Rendering::Render_Texture(gameobject.transform, texture, cutout);
}