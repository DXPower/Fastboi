#include "Resources.h"
#include "Application.h"
#include "Input.h"
#include "Rect.h"
#include "Rendering.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <unordered_map>
#include "Utility.h"

using namespace Fastboi;

std::unordered_map<const char*, Texture, cstring_hasher> textures;

constexpr const char* imagePath = "res/images/";

void CopyTexture(const Texture& src, Texture& dst); // Does no exception checks. Only used

Texture::Texture(const Vec<int>& size, int access, uint32_t format)
 : size(size), access(access), format(format)
 , sdl_texture(SDL_CreateTexture(Application::gRenderer, format, access, size.x, size.y))
{ }

Texture::Texture(SDL_Texture* sdlt) : sdl_texture(sdlt) {
    if (sdlt != nullptr)
        SDL_QueryTexture(sdlt, &format, &access, &size.x, &size.y);
}

Texture::Texture(const Texture& copy) {
    if (access != SDL_TEXTUREACCESS_TARGET)
        Application::ThrowRuntimeException("Copy nontarget texture", Application::COPY_NONTARGET_TEXTURE);

    format = copy.format;
    access = copy.access;
    size = copy.size;

    sdl_texture = SDL_CreateTexture(Application::gRenderer, format, access, size.x, size.y);
    CopyTexture(copy, *this);
}

Texture::~Texture() {
    SDL_DestroyTexture(sdl_texture);
}

Texture& Texture::operator=(const Texture& copy) {
    if (access != SDL_TEXTUREACCESS_TARGET)
        Application::ThrowRuntimeException("Copy nontarget texture", Application::COPY_NONTARGET_TEXTURE);

    SDL_DestroyTexture(sdl_texture);

    format = copy.format;
    access = copy.access;
    size = copy.size;

    sdl_texture = SDL_CreateTexture(Application::gRenderer, format, access, size.x, size.y);
    CopyTexture(copy, *this);

    return *this;
}

void Texture::Recreate(const Vec<int>& size, int access, uint32_t format) {
    if (sdl_texture != nullptr) {
        SDL_DestroyTexture(sdl_texture);
    }

    sdl_texture = SDL_CreateTexture(Application::gRenderer, format, access, size.x, size.y);

    this->size = size;
    this->access = access;
    this->format = format;
}

void CopyTexture(const Texture& src, Texture& dest) {
    Rect rect(0, 0, src.GetSize().x, src.GetSize().y);

    SDL_SetRenderTarget(Application::gRenderer, dest.GetSDL_Texture());

    SDL_RenderCopy(Application::gRenderer, src.GetSDL_Texture(), &rect, &rect);
    SDL_SetRenderTarget(Application::gRenderer, nullptr);
}

void Resources::LoadImage(const char* key, const char* filename) {
    char* pathBuffer = new char[strlen(imagePath) + strlen(filename) + 10];
    sprintf(pathBuffer, "%s%s", imagePath, filename);

    SDL_Surface* surface = IMG_Load(pathBuffer);

    if (surface == nullptr) {
        printf("Could not load %s\n", pathBuffer);
        throw -1;
    }

    // Create texture from surface optimized for global SDL renderer
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Application::gRenderer, surface);
    SDL_FreeSurface(surface);

    if (texture == nullptr) {
        printf("Could not create texture from %s\n", pathBuffer);

        throw -1;
    }

    printf("\nCreating texture %s: %p\n", key, texture);
    textures.emplace(key, texture);
}

const Texture& Resources::GetTexture(const char* key) {
    printf("Getting texture %s\n", key);
    return textures.at(key);
}

void Resources::Cleanup() {
    textures.clear();
}