#pragma once

#include "SDL/SDL.h"
#include <type_traits>

namespace Fastboi {
    struct Rect final : SDL_Rect {
        Rect(int x, int y, int w, int h) {
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
        };
    };

    struct RectF final : SDL_FRect {
        RectF(float x, float y, float w, float h) {
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
        };
    };

    template<typename R>
    auto ToRect(const R& rect) {
        if constexpr (std::is_same_v<R, SDL_Rect>) {
            return Rect(rect.x, rect.y, rect.w, rect.h);
        } else if constexpr (std::is_same_v<R, SDL_FRect>) {
            return RectF(rect.x, rect.y, rect.w, rect.h);
        }
    }
};