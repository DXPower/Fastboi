#pragma once

#include "ScreenspaceExpression.h"

namespace Fastboi {
    struct Transform;

    namespace UI {
        using RelativeFunc_t = std::function<float(Size selfSize, const Transform& parent)>;

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

            Vec<float> CalculateAnchor(Size selfSize, const Transform& parent) const { return { anchor.x(selfSize, parent), anchor.y(selfSize, parent) }; }
            Vec<float> CalculateScale(Size selfSize, const Transform& parent) const  { return { scale.x(selfSize, parent),  scale.y(selfSize, parent) }; }

            /*
             * New transform is:
             * Size: self.size * scale(self.size, self.parent)
             * Position: self.position + anchor(newSize, self.parent)
            */
            Transform RelativizeTransform(const Transform& self) const;
        };
    }
}