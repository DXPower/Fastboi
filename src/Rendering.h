#pragma once

#include "FastboiCore.h"
#include "Application.h"
#include "Camera.h"
#include "Rect.h"
#include "SDL/SDL.h"
#include <memory>
#include <type_traits>

using Texture = SDL_Texture;

namespace Fastboi {
    namespace Rendering {
        enum FillType {
            FILLED,
            UNFILLED
        };

        using std::is_same_v;
        using Application::gRenderer;

        
        // Gets left corner of a transform (assuming transform.position is center), ignoring rotation
        inline Position GetLeftCorner(const Transform& transform) {
            return transform.position - (transform.size / 2);
        }

        template<FillType fill>
        void Render_Rect(const RectF& rect) {
            if constexpr (fill == FillType::FILLED) {
                SDL_RenderFillRectF(gRenderer, &rect);
            } else {
                SDL_RenderDrawRectF(gRenderer, &rect);
            }
        }

        template<FillType fill>
        void Render_Rect(const Transform& transform) {
            Position screenPos = Fastboi::camera.WorldToScreenPos(GetLeftCorner(transform));
            SDL_FRect rect{ screenPos.x, screenPos.y, transform.size.x, transform.size.y };

            if constexpr (fill == FillType::FILLED) {
                SDL_RenderFillRectF(gRenderer, &rect);
            } else {
                SDL_RenderDrawRectF(gRenderer, &rect);
            }
        }

        template<FillType fill>
        void Render_Rect(const std::unique_ptr<Transform>& transform)  {
            Render_Rect<fill>(*transform);
        };

        template<FillType fill>
        void RenderScreen_Rect(const Transform& transform) {
            Position leftCorner = GetLeftCorner(transform);
            SDL_FRect rect{ leftCorner.x, leftCorner.y, transform.size.x, transform.size.y};
            
            if constexpr (fill == FillType::FILLED) {
                SDL_RenderFillRectF(gRenderer, &rect);
            } else {
                SDL_RenderDrawRectF(gRenderer, &rect);
            }
        }
        
        template<FillType fill>
        void RenderScreen_Rect(const std::unique_ptr<Transform>& transform)  {
            RenderScreen_Rect<fill>(*transform);
        };

        void Render_Line(const Position& worldA, const Position& worldB);

        void Render_Texture(const Transform& transform, const Texture* texture, const Rect& cutout);

        inline void Render_Texture(const std::unique_ptr<Transform>& transform, const Texture* texture, const Rect& cutout) {
            Render_Texture(*transform, texture, cutout);
        }

        void Render_Texture(const Transform& transform, const Texture* texture);

        inline void Render_Texture(const std::unique_ptr<Transform>& transform, const Texture* texture) {
            Render_Texture(*transform, texture);
        }

        void Render_TextureTarget(const Texture* src, const Texture* dest, const Rect& destRect);

        inline void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            SDL_SetRenderDrawColor(gRenderer, r, g, b, a);
        }

        inline Texture* CreateTexture(uint32_t format, SDL_TextureAccess access, Size size) {
            return SDL_CreateTexture(gRenderer, format, access, (int) size.x, (int) size.y);
        }

        Vec<int> GetTextureSize(const Texture* texture);
    }
}