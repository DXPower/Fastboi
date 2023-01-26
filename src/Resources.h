#pragma once

#include <string_view>
#include "Texture.h"
#include "Vec.h"

struct SDL_Surface;
struct SDL_Texture;

namespace SoLoud {
    class Wav;
    class Soloud;
}

namespace Fastboi {
    struct WindowResizeEvent;

    using SoLoud::Wav;
    using SoLoud::Soloud;

    namespace Resources {
        // Stores image file as texture to be used later
        void LoadImage(std::string_view key, std::string_view filename);
        CTextureRef GetTexture(std::string_view key);

        void LoadSound(std::string_view key, std::string_view filename);
        Wav& GetSound(std::string_view key);

        Soloud& GetSoloud();

        void Cleanup();
    }
}