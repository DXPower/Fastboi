#pragma once

#include <mutex>
#include "Vec.h"

struct SDL_Surface;
struct SDL_Texture;


namespace Fastboi {
    struct WindowResizeEvent;

    namespace Input {
        void PollEvents();
    };

    struct Texture final{
        private:
        Vec<int> size;
        int access;
        uint32_t format;

        mutable SDL_Texture* sdl_texture;

        public:
        Texture(const Vec<int>& size, int access, uint32_t format);
        Texture(SDL_Texture* sdlt);
        Texture(const Texture& copy);
        ~Texture();

        Texture& operator=(const Texture& copy);

        const Vec<int>& GetSize() const { return size; }
        int GetAccess() const { return access; }
        uint32_t GetFormat() const { return format; }
        SDL_Texture* GetSDL_Texture() const { return sdl_texture; }

        void Recreate(const Vec<int>& size, int access, uint32_t format);
    };

    namespace Resources {
        // Stores image as texture to be used later
        void LoadImage(const char* key, const char* filename);
        const Texture& GetTexture(const char* key);

        void Cleanup();
    }
}