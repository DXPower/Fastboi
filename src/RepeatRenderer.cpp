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

    Input::resizeSignal.connect<&RepeatRenderer::WindowSizeChanged>(this);   

    lastSize = gameobject.transform->size;                              
}

RepeatRenderer::~RepeatRenderer() {
    printf("Destroying repeat renderer.");

    Input::resizeSignal.disconnect<&RepeatRenderer::WindowSizeChanged>(this);   
}

void RepeatRenderer::Render() {
    if (lastSize != gameobject.transform->size || repeatTexture.GetSDL_Texture() == nullptr) {
        lastSize = gameobject.transform->size;
        CreateRepeatTexture(lastSize);
    }

    Rendering::Render_Texture(gameobject.transform, repeatTexture);
}

void RepeatRenderer::CreateRepeatTexture(const Size& newSize) {
    repeatTexture.Recreate(gameobject.transform->size, SDL_TEXTUREACCESS_TARGET, baseTexture.GetFormat());                                       

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
    printf("Repeat renderer: Window size changed.\n");
    CreateRepeatTexture(gameobject.transform->size);
}