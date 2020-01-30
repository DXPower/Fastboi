#pragma once

#include "Vec.h"

struct SDL_Surface;
struct SDL_Texture;


namespace Fastboi {
    struct Texture;
    struct WindowResizeEvent;

    namespace Input {
        void PollEvents();
    };

    namespace Resources {
        // Stores image as texture to be used later
        void LoadImage(const char* key, const char* filename);
        const Texture& GetTexture(const char* key);

        void Cleanup();
    }
}