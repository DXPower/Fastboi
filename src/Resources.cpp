#include "Resources.h"
#include "Application.h"
#include "Input.h"
#include "Rect.h"
#include "Rendering.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "Texture.h"
#include <unordered_map>
#include "Utility.h"

using namespace Fastboi;

std::unordered_map<const char*, Texture, cstring_hasher> textures;
constexpr const char* imagePath = "res/images/";

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