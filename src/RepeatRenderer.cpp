#include "RepeatRenderer.h"
#include "circular_vector.h"

using namespace Fastboi;
using namespace Fastboi::Rendering;
using namespace Components;

RepeatRenderer::RepeatRenderer(Gameobject& gameobject, RenderData data, const char* textureName, const Size& tileSize)
: Renderer(gameobject, data)
, baseTexture(Fastboi::Resources::GetTexture(textureName))
, repeatTexture(nullptr)
, tileSize(tileSize) { }

void RepeatRenderer::Start() {
    Renderer::Start();
    
    observer = &gameobject.AddComponent<ChangeObserver<Size>>(&gameobject.transform->size);
    observer->signal.connect<&RepeatRenderer::RenderRepeatTexture>(this);  

    Input::resizeSignal.connect<&RepeatRenderer::WindowSizeChanged>(this);   

    RenderRepeatTexture(gameobject.transform->size);                               
}

RepeatRenderer::~RepeatRenderer() {
    printf("Destroying repeat renderer.");

    Input::resizeSignal.disconnect<&RepeatRenderer::WindowSizeChanged>(this);   
    gameobject.RemoveComponent<ChangeObserver<Size>>();
}

void RepeatRenderer::Render() {
    Rendering::Render_Texture(gameobject.transform, repeatTexture);
    // Vec<int> r, b;
    // SDL_QueryTexture(repeatTexture, nullptr, nullptr, &r.x, &r.y);
    // SDL_QueryTexture(baseTexture, nullptr, nullptr, &b.x, &b.y);
    // printf("Repeat texture: %p %i %i, Base texture: %p %i %i\n", &r, r.x, r.y, &b, b.x, b.y);
}

void RepeatRenderer::RecreateRepeatTexture() {
    uint32_t format = 0;
    int access, w, h;

    // if (repeatTexture != nullptr)
    //     SDL_DestroyTexture(repeatTexture);

    // SDL_QueryTexture(baseTexture, &format, &access, &w, &h);
    // repeatTexture = Rendering::CreateTexture(format
                                        //    , SDL_TEXTUREACCESS_TARGET
                                        //    , gameobject.transform->size);
    // repeatTexture = Rendering::Texture(gameobject.transform->size, SDL_TEXTUREACCESS_TARGET, format); 

    repeatTexture.Recreate(gameobject.transform->size, SDL_TEXTUREACCESS_TARGET, baseTexture.GetFormat());                                       
}

void RepeatRenderer::RenderRepeatTexture(const Size& newSize) {
    RecreateRepeatTexture(); // The texture needs to be as big as the new size

    const Size& size = gameobject.transform->size; // Using Size& size instead of newSize for cache locality

    // Count tiles in each direction. +1 because we are guarenteeing that one tile is exactly centered,
    //   meaning there is always an odd number of tiles in each direction.
    //. Pros of fixing tile to center: Increasing size standardly (relative from center) keeps texture fixed.
    //. Cons: Increasing size relative to any other point (say, top left corner) makes texture move as it increases.
    Vec<int> numEachDir(ceil(size.x / tileSize.x) + 1, ceil(size.y / tileSize.y) + 1);

    // Calculate center, then the origin of the tile that covers the upper left corner
    Vec<int> center = (Vec<int>) (size / 2);
    Vec<int> drawOrigin = center - ((Vec<int>) tileSize * (numEachDir / 2));

    // Basic tiled coordinate loop using drawOrigin as offset
    for (int ix = 0; ix < numEachDir.x; ix++) {
        for (int iy = 0; iy < numEachDir.y; iy++) {
            Vec<int> rectOrigin(drawOrigin.x + (ix * tileSize.x), drawOrigin.y + (iy * tileSize.y));
            Vec<int> rectSize(tileSize.x, tileSize.y);

            Rect drawRect(rectOrigin.x, rectOrigin.y, rectSize.x, rectSize.y);
            Rendering::Render_TextureTarget(baseTexture, repeatTexture, drawRect);
        }
    }
}

void RepeatRenderer::WindowSizeChanged(const Fastboi::WindowResizeEvent& e) {
    RenderRepeatTexture(gameobject.transform->size);
}