#include "Tile.h"
#include "FastboiComps.h"
#include <random>

using namespace Fastboi;
using namespace Fastboi::Components;
using namespace CS;

Tile::Tile(TileLayer& layer, Veci pos, const Rect& cutout)
 : Tile(layer, pos, cutout, 0_deg) { }

Tile::Tile(TileLayer& layer, Veci pos, const Rect& cutout, Degree rot)
 : layer(&layer), position(pos), cutout(cutout), rotation(rot) { }

TileObject::TileObject(TileLayer& layer, Veci pos, const Rect& cutout, Gameobject& gameobject)
 : Tile(layer, pos, cutout), gameobject(&gameobject) { }

void Tile::SetCutout(const Rect& cutout) {
    this->cutout = cutout;
    
    layer->UpdateTexture();
}

const Rect& Tile::GetCutout() const {
    return cutout;
}

void Tile::SetRotation(Degree deg) {
    rotation = deg;

    layer->UpdateTexture();
}

Degree Tile::GetRotation() const {
    return rotation;
}

const Veci& Tile::GetCoords() const {
    return position;
}

TileLayer::TileLayer(int width, int height, Veci tileSize)
 : size(width, height)
 , tileSize(tileSize)
 , layerTexture(Texture(nullptr)) {

    tiles.reserve(width * height);

    const Texture& spritesheet = Resources::GetTexture("TileSpritesheet");

    layerTexture.Recreate(Vec<int>(width, height) * (Vec<int>) tileSize, SDL_TEXTUREACCESS_TARGET, spritesheet.GetFormat());
    SDL_SetTextureBlendMode(layerTexture.GetSDL_Texture(), SDL_BLENDMODE_BLEND);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Rect cutout = Rect(864, 224, 64, 64); // Default tile is just some random squares

            tiles.push_back(Tile(*this, Veci(x, y), cutout));
        }
    }

    UpdateTexture();
}

void TileLayer::UpdateTexture() {
    if (isLocked) return;

    const Texture& tileSpritesheet = Resources::GetTexture("TileSpritesheet");

    for (const Tile& tile : tiles) {
        const Veci tileCorner = tileSize * tile.GetCoords();
        // const Rect destRect = Rect(tileCorner.x, tileCorner.y, tileSize.x, tileSize.y);
        const Rect& cutout = tile.GetCutout();

        Transform destT = Transform(tileCorner, tileSize, tile.GetRotation());

        Rendering::Render_TextureTarget(tileSpritesheet, layerTexture, destT, cutout);
    }

    printf("Done updating texture!\n");
}

Tile& TileLayer::AccessTile(int x, int y) const {
    return tiles[size.x * y + x];
}

decltype(TileLayer::tiles)& TileLayer::GetTiles() {
    return tiles;
}

const decltype(TileLayer::tiles)& TileLayer::GetTiles() const {
    return tiles;
}

const Texture& TileLayer::GetTexture() const {
    return layerTexture;
}

void TileLayer::LockTexture() {
    isLocked = true;
}

void TileLayer::UnlockTexture() {
    isLocked = false;
}

void TileLayer::Blueprint(Gameobject& go, const Position& origin, const Veci& size, const Veci& tileSize) {
    Veci textureSize = size * tileSize;
    
    go.name = "TileLayerGO";
    go.AddComponent<Transform>(origin, (Size) textureSize, 0_deg);

    const TileLayer& layer = go.AddComponent<TileLayer>(size, tileSize);
    const Texture& layerTexture = layer.GetTexture();

    go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::GROUND, 0), layer.GetTexture(), Rect(0, 0, layerTexture.GetSize().x, layerTexture.GetSize().y));
}