#include "Rendering.h"
#include "Resources.h"
#include "Utility.h"

using namespace Fastboi;
using namespace Resources;

#define ccast_tx(t) const_cast<Texture*>(t)

void Rendering::Render_Line(const Position& worldA, const Position& worldB) {
    Position screenA = Fastboi::camera.WorldToScreenPos(worldA);
    Position screenB = Fastboi::camera.WorldToScreenPos(worldB);

    SDL_RenderDrawLineF(gRenderer, screenA.x, screenA.y, screenB.x, screenB.y);
}

void Rendering::Render_Texture(const Transform& transform, const Texture* texture, const SDL_Rect& cutout) {
    Position screenPos = Fastboi::camera.WorldToScreenPos(GetLeftCorner(transform));

    SDL_FRect dest { screenPos.x, screenPos.y, transform.size.x, transform.size.y };

    if (transform.GetRotation() == 0.f)
        SDL_RenderCopyF(gRenderer, ccast_tx(texture), &cutout, &dest);
    else {
        SDL_RenderCopyExF(gRenderer, ccast_tx(texture), &cutout, &dest, transform.GetRotation(), nullptr, SDL_FLIP_NONE);
    }
}

void Rendering::Render_Texture(const Transform& transform, const Texture* texture) {
    Vec<int> size(GetTextureSize(texture)); // Get texture size
    Render_Texture(transform, texture, (SDL_Rect) { 0, 0, size.x, size.y });
}

void Rendering::Render_TextureTarget(const Texture* src, const Texture* dest, const SDL_Rect& destRect) {
    SDL_SetRenderTarget(gRenderer, ccast_tx(dest)); // Target the destination texture

    Vec<int> srcSize(GetTextureSize(src)); // Get size of src texture
    SDL_Rect srcRect { 0, 0, srcSize.x, srcSize.y };

    SDL_RenderCopy(gRenderer, ccast_tx(src), &srcRect, &destRect);

    SDL_SetRenderTarget(gRenderer, nullptr); // Reset render target to be default (gRenderer)
}

Vec<int> Rendering::GetTextureSize(const Texture* texture) {
    uint32_t format;
    int access, w, h;

    SDL_QueryTexture(const_cast<Texture*>(texture), &format, &access, &w, &h);

    return Vec<int>(w, h);
}

#undef ccast_tx