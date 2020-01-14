#pragma once

struct SDL_Surface;
struct SDL_Texture;

namespace Fastboi {
    namespace Resources {
        using Texture = SDL_Texture;

        // Stores image as texture to be used later
        void LoadImage(const char* key, const char* filename);
        Texture* GetTexture(const char* key);

        void Cleanup();
    }
}