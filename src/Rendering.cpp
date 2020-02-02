#include "Rendering.h"
#include "Input.h"
#include <mutex>
#include "Resources.h"
#include "Texture.h"
#include "Utility.h"

using namespace Fastboi;
using namespace Resources;
using namespace Rendering;

void Rendering::Render_Line(const Position& worldA, const Position& worldB) {
    Position screenA = Fastboi::camera.WorldToScreenPos(worldA);
    Position screenB = Fastboi::camera.WorldToScreenPos(worldB);

    SDL_RenderDrawLineF(gRenderer, screenA.x, screenA.y, screenB.x, screenB.y);
}

void Rendering::Render_Texture(const Transform& transform, const Texture& texture, const Rect& cutout) {
    RectF dest = WorldTransformToScreenRect(transform);

    if (transform.GetRotation() == 0.f)
        SDL_RenderCopyF(gRenderer, texture.GetSDL_Texture(), &cutout, &dest);
    else {
        SDL_RenderCopyExF(gRenderer, texture.GetSDL_Texture(), &cutout, &dest, transform.GetRotation(), nullptr, SDL_FLIP_NONE);
    }
}

void Rendering::Render_Texture(const Transform& transform, const Texture& texture) {
    const Vec<int>& size = texture.GetSize();
    Render_Texture(transform, texture, Rect(0, 0, size.x, size.y));
}

void Rendering::RenderScreen_Texture(const Transform& transform, const Texture& texture, const Rect& cutout) {
    Position leftCorner = GetLeftCorner(transform);
    RectF dest = RectF(leftCorner.x, leftCorner.y, transform.size.x, transform.size.y);

    if (transform.GetRotation() == 0.f)
        SDL_RenderCopyF(gRenderer, texture.GetSDL_Texture(), &cutout, &dest);
    else {
        SDL_RenderCopyExF(gRenderer, texture.GetSDL_Texture(), &cutout, &dest, transform.GetRotation(), nullptr, SDL_FLIP_NONE);
    }
}

void Rendering::RenderScreen_Texture(const Transform& transform, const Texture& texture) {
    const Vec<int>& size = texture.GetSize();
    RenderScreen_Texture(transform, texture, Rect(0, 0, size.x, size.y));
}




void Rendering::Render_TextureTarget(const Texture& src, const Texture& dest, const Rect& destRect) {
    SDL_SetRenderTarget(gRenderer, dest.GetSDL_Texture()); // Target the destination texture

    Vec<int> srcSize = src.GetSize();
    SDL_Rect srcRect { 0, 0, srcSize.x, srcSize.y };

    SDL_RenderCopy(gRenderer, src.GetSDL_Texture(), &srcRect, &destRect);

    SDL_SetRenderTarget(gRenderer, nullptr); // Reset render target to be default (gRenderer)
}
