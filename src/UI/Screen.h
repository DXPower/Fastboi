#pragma once

#include "ScreenspaceExpression.h"

namespace Fastboi {
    class Transform;

    namespace UI {
        using RelativeFunc_t = std::function<float(const Transform& parent)>;

        struct ScreenspaceCoord {
            RelativeFunc_t x;
            RelativeFunc_t y;
        };

        using Anchor = ScreenspaceCoord;
        using Scale = ScreenspaceCoord;

        struct Screen {
            constexpr static auto DefaultAnchorX = ScreenWidth / 2.f; ;
            constexpr static auto DefaultAnchorY = ScreenHeight / 2.f; ;
            constexpr static auto DefaultScale = ScreenConst(1.f);

            inline static const Transform nullTransform;

            ScreenspaceCoord anchor { DefaultAnchorX, DefaultAnchorY };
            ScreenspaceCoord scale { DefaultScale, DefaultScale };

            Screen(const ScreenspaceCoord& anchor) : anchor(anchor) { }
            Screen(const ScreenspaceCoord& anchor, const ScreenspaceCoord& scale) : anchor(anchor), scale(scale) { }

            Vec<float> CalculateAnchor(const Transform& parent) const { return { anchor.x(parent), anchor.y(parent) }; }
            Vec<float> CalculateAnchor() const { return CalculateAnchor(nullTransform); }
            Vec<float> CalculateScale(const Transform& parent) const  { return { scale.x(parent),  scale.y(parent) }; }
            Vec<float> CalculateScale() const  { return CalculateScale(nullTransform); }

            Transform RelativizeTransform(const Transform& self) const;
        };
    }
}