#include "Texture.h"
#include "Application.h"
#include "FastboiCore.h"
#include <queue>
#include "Rect.h"
#include "SDL/SDL.h"

using namespace Fastboi;

struct TextureData {
    Vec<int> size;
    int access;
    uint32_t format;

    std::promise<SDL_Texture*>* prom;
};

std::queue<TextureData> texturesToCreate;
std::mutex textureCreationMtx;

void CopyTexture(const Texture& src, Texture& dst); // Does no exception checks. Only used

Texture::Texture(const Vec<int>& size, int access, uint32_t format)
 : size(size), access(access), format(format)
 , sdl_texture(CreateSDL_Texture(size, format, access))
{ }

Texture::Texture(SDL_Texture* sdlt) {
    if (sdlt != nullptr) {
        std::promise<SDL_Texture*> prom;
        std::shared_future<SDL_Texture*> fut(prom.get_future());

        prom.set_value(sdlt);
        sdl_texture = std::move(fut);

        SDL_QueryTexture(sdlt, &format, &access, &size.x, &size.y);
    } else {
        sdl_texture = std::nullopt;
    }
}

Texture::Texture(const Texture& copy) {
    if (access != SDL_TEXTUREACCESS_TARGET)
        Application::ThrowRuntimeException("Copy nontarget texture", Application::COPY_NONTARGET_TEXTURE);

    format = copy.format;
    access = copy.access;
    size = copy.size;

    sdl_texture = CreateSDL_Texture(size, access, format);
    CopyTexture(copy, *this);
}

Texture::~Texture() {
    SDL_Texture* t = GetSDL_Texture();

    if (t != nullptr) {
        SDL_DestroyTexture(t);
    }
}

Texture& Texture::operator=(const Texture& copy) {
    if (access != SDL_TEXTUREACCESS_TARGET)
        Application::ThrowRuntimeException("Copy nontarget texture", Application::COPY_NONTARGET_TEXTURE);

    if (GetSDL_Texture() != nullptr)
        SDL_DestroyTexture(GetSDL_Texture());

    format = copy.format;
    access = copy.access;
    size = copy.size;

    sdl_texture = CreateSDL_Texture(size, access, format);
    CopyTexture(copy, *this);

    return *this;
}

void Texture::Recreate(const Vec<int>& size, int access, uint32_t format) {
    if (GetSDL_Texture() != nullptr) {
        SDL_DestroyTexture(GetSDL_Texture());
    }

    sdl_texture = CreateSDL_Texture(size, access, format);

    this->size = size;
    this->access = access;
    this->format = format;
}

std::shared_future<SDL_Texture*> Texture::CreateSDL_Texture(const Vec<int>& size, int access, uint32_t format) {
    std::promise<SDL_Texture*>* prom = new std::promise<SDL_Texture*>();
    std::shared_future<SDL_Texture*> fut(prom->get_future());
    
    // SDL_Texture must always be made/handled by Rendering thread. This is the quick-way out.
    if (Fastboi::IsRenderingThread()) {
        SDL_Texture* t = SDL_CreateTexture(Application::gRenderer, format, access, size.x, size.y);
        printf("Texture made: %p\n", t);
        prom->set_value(t);
        delete prom;
    } else {
        // Else, if called on separate thread, we need to enqueue it to be made by rendering thread
        printf("Queueing texture creation\n");
        TextureData td { size, access, format, prom };
        std::lock_guard<std::mutex> lock(textureCreationMtx);
        texturesToCreate.push(td);
    }

    return fut;
}

void Texture::CreateQueuedTextures() {
    if (texturesToCreate.size() == 0) return;

    std::lock_guard<std::mutex> lock(textureCreationMtx);

    while (texturesToCreate.size() != 0) {
        TextureData& td = texturesToCreate.front();

        td.prom->set_value(SDL_CreateTexture(Application::gRenderer, td.format, td.access, td.size.x, td.size.y));
        printf("Created texture of size: %i %i\n", td.size.x, td.size.y);
        delete td.prom;

        texturesToCreate.pop();
    }
}

void CopyTexture(const Texture& src, Texture& dest) {
    Rect rect(0, 0, src.GetSize().x, src.GetSize().y);

    SDL_SetRenderTarget(Application::gRenderer, dest.GetSDL_Texture());

    SDL_RenderCopy(Application::gRenderer, src.GetSDL_Texture(), &rect, &rect);
    SDL_SetRenderTarget(Application::gRenderer, nullptr);
}
