#include "Resources.h"
#include "Application.h"
#include "Exceptions.h"
#include "Input.h"
#include "Rect.h"
#include "Rendering.h"
#include "SDL/SDL.h"
#include "SDL/SDL_blendmode.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_render.h"
#include "SDL/SDL_surface.h"
#include "soloud.h"
#include "soloud_wav.h"
#include "Texture.h"
#include <unordered_map>
#include "Utility.h"
#include <string>

#undef LoadImageA
#undef LoadImage

using namespace Fastboi;

static std::unordered_map<std::string, Texture> textures;
static std::unordered_map<std::string, Wav> sounds;

constexpr std::string_view imagePath = "res/images/";
constexpr std::string_view soundPath = "res/sound/";

Soloud gSoloud;

void Resources::LoadImage(std::string_view key, std::string_view filename) {
    auto path = std::string(imagePath) + std::string(filename);

    SDL_Surface* surface = IMG_Load(path.c_str());

    if (surface == nullptr) {
        Application::ThrowRuntimeException("Could not load image " + path, Application::LOAD_FAILURE);
    }

    // Create texture from surface optimized for global SDL renderer
    SDL_Texture* texture = SDL_CreateTextureFromSurface(Application::gRenderer, surface);
    SDL_FreeSurface(surface);

    if (texture == nullptr) {
        Application::ThrowRuntimeException("Could not create texture from " + path, Application::LOAD_FAILURE);
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // printf("\nCreating texture %s: %p\n", key, texture);
    textures.emplace(key, texture);
}

CTextureRef Resources::GetTexture(std::string_view key) {
    // printf("Getting texture %s\n", key);

    // printf("textures size: %i\n", textures.size());

    // if (textures.find(key) == textures.end())
    //     printf("Doesn't exist!\n");

    return textures.at(std::string(key));
}

void Resources::Cleanup() {
    textures.clear();
}

void Resources::LoadSound(std::string_view key, std::string_view filename) {
    auto path = std::string(soundPath) + std::string(filename);

    sounds.emplace(std::piecewise_construct, std::make_tuple(key), std::make_tuple());

    Wav& w = GetSound(key);
    SoLoud::result res = w.load(path.c_str());
}

Wav& Resources::GetSound(std::string_view key) {
    return sounds.at(std::string(key));
}

Soloud& Resources::GetSoloud() {
    return gSoloud;
}
