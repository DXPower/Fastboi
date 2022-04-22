#include "Resources.h"
#include "Application.h"
#include "Input.h"
#include "Rect.h"
#include "Rendering.h"
#include "SDL/SDL.h"
#include "SDL/SDL_blendmode.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_render.h"
#include "SDL/SDL_surface.h"
#include "soloud/soloud.h"
#include "soloud/soloud_wav.h"
#include "Texture.h"
#include <unordered_map>
#include "Utility.h"

using namespace Fastboi;

std::unordered_map<const char*, Texture, cstring_hasher, cstring_eq> textures;
std::unordered_map<const char*, Wav, cstring_hasher, cstring_eq> sounds;

constexpr const char* imagePath = "res/images/";
constexpr const char* soundPath = "res/sound/";

Soloud gSoloud;

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

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    // printf("\nCreating texture %s: %p\n", key, texture);
    textures.emplace(key, texture);
}

const Texture& Resources::GetTexture(const char* key) {
    // printf("Getting texture %s\n", key);

    // printf("textures size: %i\n", textures.size());

    // if (textures.find(key) == textures.end())
    //     printf("Doesn't exist!\n");

    return textures.at(key);
}

void Resources::Cleanup() {
    textures.clear();
}

void Resources::LoadSound(const char* key, const char* filename) {
    char* pathBuffer = new char[strlen(soundPath) + strlen(filename) + 10];
    sprintf(pathBuffer, "%s%s", soundPath, filename);

    sounds.emplace(std::piecewise_construct, std::make_tuple(key), std::make_tuple());
    Wav& w = GetSound(key);
    SoLoud::result res = w.load(pathBuffer);
    // printf("Path: %s Sound loaded: %p. Result: %u\n", pathBuffer, w.mData, res);
}

Wav& Resources::GetSound(const char* key) {
    return sounds.at(key);
}

Soloud& Resources::GetSoloud() {
    return gSoloud;
}