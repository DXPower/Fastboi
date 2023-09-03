#pragma once

#include <future>
#include <optional>
#include "Vec.h"

struct SDL_Texture;

namespace Fastboi {
    void Render();

    class Texture final {
        struct Deleter {
            void operator()(SDL_Texture* t);
        };
        
        Vec<int> size{};
        int access{};
        uint32_t format{};

        using PointerStore_t = std::unique_ptr<SDL_Texture, Deleter>;
        PointerStore_t sdl_texture{};

        public:
        Texture() = default;;
        Texture(const Vec<int>& size, int access, uint32_t format);
        Texture(SDL_Texture* sdlt);

        const Vec<int>& GetSize() const { return size; }
        int GetAccess() const { return access; }
        uint32_t GetFormat() const { return format; }
        SDL_Texture* GetSDL_Texture() const { return sdl_texture.get(); }

        private:
        static PointerStore_t CreateSDL_Texture(const Vec<int>& size, int access, uint32_t format);
    };

    using TextureRef = Texture&;
    using CTextureRef = const Texture&;

    using TexturePtr = Texture*;
    using CTexturePtr = const Texture*;
}