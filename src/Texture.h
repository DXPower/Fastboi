#pragma once

#include <future>
#include <optional>
#include "Vec.h"

struct SDL_Texture;

namespace Fastboi {
    void Render();

    struct Texture final{
        private:
        Vec<int> size;
        int access;
        uint32_t format;

        mutable std::optional<std::shared_future<SDL_Texture*>> sdl_texture;

        public:
        Texture(const Vec<int>& size, int access, uint32_t format);
        Texture(SDL_Texture* sdlt);
        Texture(const Texture& copy);
        ~Texture();

        Texture& operator=(const Texture& copy);

        const Vec<int>& GetSize() const { return size; }
        int GetAccess() const { return access; }
        uint32_t GetFormat() const { return format; }
        SDL_Texture* GetSDL_Texture() const { 
            return sdl_texture.has_value() ? sdl_texture.value().get() : nullptr; 
        }

        void Recreate(const Vec<int>& size, int access, uint32_t format);

        private:
        static std::shared_future<SDL_Texture*> CreateSDL_Texture(const Vec<int>& size, int access, uint32_t format);
        static void CreateQueuedTextures();

        friend void Fastboi::Render();
    };
}