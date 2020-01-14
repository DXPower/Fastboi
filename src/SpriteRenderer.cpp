#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(Gameobject& gameobject, RenderData data, const char* textureName, const Rect& cutout)
    : Renderer(gameobject, data)
    , texture(Fastboi::Resources::GetTexture(textureName))
    , cutout(cutout) {
    printf("Sprite renderer made\n");
}

SpriteRenderer::~SpriteRenderer() { }

void SpriteRenderer::Render() {
    Fastboi::Rendering::Render_Texture(gameobject.transform, texture, cutout);
}