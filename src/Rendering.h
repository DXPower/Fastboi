#pragma once

#include "FastboiCore.h"
#include "Application.h"
#include "Camera.h"
#include "ColorComp.h"
#include "Rect.h"
#include "SDL/SDL.h"
#include <memory>
#include <type_traits>

namespace Fastboi {
    struct Texture;

    namespace Rendering {
        enum FillType {
            FILLED,
            UNFILLED
        };

        using std::is_same_v;
        using Application::gRenderer;

        inline Position GetLeftCorner(const Transform& transform) {
            return transform.position - (transform.size / 2.f);
        }
        
        // Gets left corner of a transform (assuming transform.position is center), ignoring rotation
        inline RectF WorldTransformToScreenRect(const Transform& t) {
            Position screenLeftCorner = Fastboi::camera.WorldToScreenPos(GetLeftCorner(t));

            return RectF(screenLeftCorner.x, screenLeftCorner.y, t.size.x * camera.zoom, t.size.y * camera.zoom);
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
            RectF rect = WorldTransformToScreenRect(transform);

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

        void Request_Render_DebugRect(const RectF& rect);
        void Render_AllDebugRects();

        template<FillType fill>
        void RenderScreen_Rect(const Transform& transform) {
            Position leftCorner = GetLeftCorner(transform);
            SDL_FRect rect = { leftCorner.x, leftCorner.y, transform.size.x, transform.size.y };

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

        void Render_Texture(const Transform& transform, const Texture& texture, const Rect& cutout);

        inline void Render_Texture(const std::unique_ptr<Transform>& transform, const Texture& texture, const Rect& cutout) {
            Render_Texture(*transform, texture, cutout);
        }

        void Render_Texture(const Transform& transform, const Texture& texture);

        inline void Render_Texture(const std::unique_ptr<Transform>& transform, const Texture& texture) {
            Render_Texture(*transform, texture);
        }

        void RenderScreen_Texture(const Transform& transform, const Texture& texture, const Rect& cutout);

        inline void RenderScreen_Texture(const std::unique_ptr<Transform>& transform, const Texture& texture, const Rect& cutout) {
            RenderScreen_Texture(*transform, texture, cutout);
        }

        void RenderScreen_Texture(const Transform& transform, const Texture& texture);

        inline void RenderScreen_Texture(const std::unique_ptr<Transform>& transform, const Texture& texture) {
            RenderScreen_Texture(*transform, texture);
        }

        void Render_TextureTarget(const Texture& src, const Texture& dest, const Rect& destRect);
        void Render_TextureTarget(const Texture& src, const Texture& dest, const Rect& destRect, const Rect& cutout);

        inline void SetColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
            SDL_SetRenderDrawColor(gRenderer, r, g, b, a);
        }

        inline void SetColor(const Components::ColorComp& color) {
            SetColor(color.r, color.g, color.b, color.a);
        }
    }
}