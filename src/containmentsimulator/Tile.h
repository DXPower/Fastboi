#pragma once

#include "Fastboi.h"
#include "Vec.h"

using namespace Fastboi;

namespace CS {
    class TileLayer;

    class Tile {
        protected:
        TileLayer* layer = nullptr;

        Veci position = Veci::zero();

        Rect cutout{0, 0, 0, 0};
        Degree rotation = 0_deg;
        
        public:
        Tile() {};
        Tile(TileLayer& layer, Veci pos, const Rect& cutout);
        Tile(TileLayer& layer, Veci pos, const Rect& cutout, Degree rotation);

        void SetCutout(const Rect& texture);
        const Rect& GetCutout() const;

        void SetRotation(Degree deg);
        Degree GetRotation() const;

        const Veci& GetCoords() const;
    };

    class TileObject final : public Tile {
        private: 
        Gameobject* gameobject = nullptr;

        TileObject() : Tile() {};
        TileObject(TileLayer& layer, Veci pos, const Rect& cutout, Gameobject& gameobject);
    };

    class TileLayer final {
        inline static const std::string tileTextureName = "TileSpritesheet";

        mutable std::vector<Tile> tiles;
        Veci size = Veci::zero();
        Veci tileSize = Veci::zero();

        Texture layerTexture;

        bool isLocked = false;

        public:
        TileLayer(int width, int height, Veci tileSize);
        TileLayer(Veci size, Veci tileSize) : TileLayer(size.x, size.y, tileSize) { };

        void UpdateTexture();
        const Texture& GetTexture() const;
        
        Tile& AccessTile(int x, int y)const;
        Tile& AccessTile(Veci pos) const { return AccessTile(pos.x, pos.y); };

        decltype(tiles)& GetTiles();
        const decltype(tiles)& GetTiles() const;

        void InitTiles(int width, int height);
        void InitTiles(Veci size) { InitTiles(size.x, size.y); };

        void LockTexture(); // Block any attempts to update the texture
        void UnlockTexture(); // Allow the texture to be updated again

        static void Blueprint(Gameobject& go, const Position& origin, const Veci& size, const Veci& tileSize);
    };
}