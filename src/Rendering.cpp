#include "Rendering.h"
#include "Input.h"
#include <mutex>
#include "Resources.h"
#include <stack>
#include "Texture.h"
#include "Utility.h"

using namespace Fastboi;
using namespace Resources;
using namespace Rendering;

std::mutex debugRectMtx;
std::stack<RectF> debugRects;

void Rendering::Render_Line(const Position& worldA, const Position& worldB) {
    Position screenA = Fastboi::camera.WorldToScreenPos(worldA);
    Position screenB = Fastboi::camera.WorldToScreenPos(worldB);

    SDL_RenderDrawLineF(gRenderer, screenA.x, screenA.y, screenB.x, screenB.y);
}

void Rendering::Request_Render_DebugRect(const RectF& rect) {
    std::lock_guard lock(debugRectMtx);
    debugRects.push(rect);
}

void Rendering::Render_AllDebugRects() {
    std::lock_guard lock(debugRectMtx);

    while (debugRects.size() != 0) {
        const RectF& rect = debugRects.top();

        Rendering::Render_Rect<Rendering::UNFILLED>(Transform(Position(rect.x, rect.y), Size(rect.w, rect.h), 0));

        debugRects.pop();
    }
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
    // SDL_SetRenderTarget(gRenderer, dest.GetSDL_Texture()); // Target the destination texture

    Vec<int> srcSize = src.GetSize();
    SDL_Rect srcRect { 0, 0, srcSize.x, srcSize.y };

    Render_TextureTarget(src, dest, destRect, ToRect(srcRect));
}

void Rendering::Render_TextureTarget(const Texture& src, const Texture& dest, const Rect& destRect, const Rect& cutout) {
    SDL_SetRenderTarget(gRenderer, dest.GetSDL_Texture()); // Target the destination texture

    SDL_RenderCopy(gRenderer, src.GetSDL_Texture(), &cutout, &destRect);

    SDL_SetRenderTarget(gRenderer, nullptr); // Reset render target to be default (gRenderer)
}

Components::ColorComp bgColor(255, 255, 255, 255);

void Rendering::SetBGColor(const Components::ColorComp& color) { bgColor = color; };
const Components::ColorComp& Rendering::GetBGColor() { return bgColor; };
