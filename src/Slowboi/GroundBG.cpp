#include "GroundBG.h"
#include "FastboiComps.h"

using namespace Fastboi;
using il = std::initializer_list<Rect>;

namespace Slowboi {
    struct GroundRenderer : public Renderer {
        std::vector<Rect> sprites; // These need to be stored because all render calls must come from Render thread... 
        const Texture& spritesheet; // TODO: Add render call requests via lambdas to avoid this?
        Texture ground;
        Size tileSize;

        bool hasDrawnGround = false;

        GroundRenderer(GORef&& go, RenderData data, const Texture& spritesheet, const Size& tileSize, const std::vector<Rect>& sprites)
        : Renderer(std::move(go), data)
        , sprites(sprites), spritesheet(spritesheet), ground(nullptr), tileSize(tileSize)
        { };

        void DrawGround() {
            printf("Rendering thread: %i\n", Fastboi::IsRenderingThread());
            ground.Recreate(gameobject().transform->size, SDL_TEXTUREACCESS_TARGET, spritesheet.GetFormat());
            hasDrawnGround = true;

            Rect drawRect(0, 0, tileSize.x, tileSize.y);
            const int nx = ground.GetSize().x / drawRect.w;
            const int ny = ground.GetSize().y / drawRect.h;

            printf("nx ny: %i %i  dw dh: %i %i\n", nx, ny, drawRect.w, drawRect.h);
            printf("SDLT: %p\n", ground.GetSDL_Texture());
            for (int i = 0; i < nx; i++) {
                drawRect.x = i * drawRect.w;

                for (int j = 0; j < ny; j++) {
                    drawRect.y = j * drawRect.h;
                    Rendering::Render_TextureTarget(spritesheet, ground, drawRect, *sprites.begin());
                    // SDL_SetRenderTarget(Rendering::gRenderer, ground.GetSDL_Texture());
                    // Rendering::RenderScreen_Rect<Rendering::FILLED>(Transform(Position(50, 50), Size(200, 300), 0));
                    // SDL_SetRenderTarget(Rendering::gRenderer, nullptr);
                }
            }
            
            // SDL_SetRenderTarget(Rendering::gRenderer, ground.GetSDL_Texture());
            // Rendering::RenderScreen_Rect<Rendering::FILLED>(Transform(Position(50, 50), Size(200, 300), 0));
            // SDL_SetRenderTarget(Rendering::gRenderer, nullptr);
            return;

        }

        void Render() override {
            if (!hasDrawnGround) {
                DrawGround();
            }

            Rendering::Render_Texture(Transform(gameobject().transform->position, gameobject().transform->size * 2.f, 0), spritesheet);
            Rendering::Render_Texture(Transform(gameobject().transform->position, gameobject().transform->size * 1.3f, 0), spritesheet);
            Rendering::Render_Texture(gameobject().transform, ground);
        }
        
        protected:
        GroundRenderer(const GroundRenderer& copy) = default;

        Renderer& Duplicate() const override {
            return *(new GroundRenderer(*this));
        }
    };
}

void Slowboi::GroundBG(Gameobject& go, const Position& pos, const Size& size, const Texture& spritesheet, const Size& tileSize, const std::vector<Rect>& sprites) {
    printf("Making GroundBG!\n");
    Rect r = *sprites.begin();
    printf("Rect begin: %i %i %i %i\n", r.x, r.y, r.w, r.h);
    go.AddComponent<Transform>(pos, size, 0);
    go.AddComponent<GroundRenderer>(RenderData(RenderOrder::GROUND), spritesheet, tileSize, sprites);

    printf("Ground renderer added!\n");
}