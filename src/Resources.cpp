#include "Resources.h"
#include "Application.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <unordered_map>
#include "Utility.h"

using namespace Fastboi;

std::unordered_map<const char*, SDL_Texture*, cstring_hasher> textures;

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

SDL_Texture* Resources::GetTexture(const char* key) {
    printf("%s: %p\n", key, textures[key]);
    return textures[key];
}

void Resources::Cleanup() {
    for (auto& [key, texture] : textures) {
        SDL_DestroyTexture(texture);
    }
}