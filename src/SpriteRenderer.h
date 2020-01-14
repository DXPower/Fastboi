#pragma once

#include "Fastboi.h"

struct SpriteRenderer final : public Renderer {
    Texture* texture;
    SDL_Rect cutout;

    /**
     * @param gameobject Pointer to gameobject owning this renderer
     * @param textureName Key used by Fastboi::Resources to load texture
     * @param cutout Rectangle of texture to render
    **/
    SpriteRenderer(Gameobject* gameobject, RenderData data, const char* textureName, const SDL_Rect& cutout);
    ~SpriteRenderer();

    void Render() override;
};