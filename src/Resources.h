#pragma once

#include "Vec.h"
#undef LoadImageA

struct SDL_Surface;
struct SDL_Texture;

namespace SoLoud {
    class Wav;
    class Soloud;
}

namespace Fastboi {
    struct Texture;
    struct WindowResizeEvent;

    using SoLoud::Wav;
    using SoLoud::Soloud;

    namespace Resources {
        // Stores image file as texture to be used later
        void LoadImage(const char* key, const char* filename);
        const Texture& GetTexture(const char* key);

        void LoadSound(const char* key, const char* filename);
        Wav& GetSound(const char* key);

        Soloud& GetSoloud();

        void Cleanup();
    }
}