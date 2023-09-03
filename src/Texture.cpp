#include "Texture.h"
#include "Application.h"
#include "FastboiCore.h"
#include <queue>
#include "Rect.h"
#include "SDL/SDL.h"

using namespace Fastboi;

// void CopyTexture(const Texture& src, Texture& dst); // Does no exception checks. Only used

Texture::Texture(const Vec<int>& size, int access, uint32_t format)
    : size(size), access(access), format(format)
    , sdl_texture(CreateSDL_Texture(size, format, access))
{ }

Texture::Texture(SDL_Texture* sdlt) {
    if (sdlt == nullptr)
        return;

    sdl_texture = PointerStore_t(sdlt, {});

    SDL_QueryTexture(sdlt, &format, &access, &size.x, &size.y);
}

auto Texture::CreateSDL_Texture(const Vec<int>& size, int access, uint32_t format) -> PointerStore_t {
    // SDL_Texture must always be made/handled by Rendering thread
    SDL_Texture* t = SDL_CreateTexture(Application::gRenderer, format, access, size.x, size.y);

    return PointerStore_t(t, {});
}

static void CopyTexture(const Texture& src, Texture& dest) {
    // if (access != SDL_TEXTUREACCESS_TARGET)
    //     Application::ThrowRuntimeException("Copy nontarget texture", Application::COPY_NONTARGET_TEXTURE);

    Rect rect(0, 0, src.GetSize().x, src.GetSize().y);

    SDL_SetRenderTarget(Application::gRenderer, dest.GetSDL_Texture());

    SDL_RenderCopy(Application::gRenderer, src.GetSDL_Texture(), &rect, &rect);
    SDL_SetRenderTarget(Application::gRenderer, nullptr);
}

void Texture::Deleter::operator()(SDL_Texture* t) {
    SDL_DestroyTexture(t);
}