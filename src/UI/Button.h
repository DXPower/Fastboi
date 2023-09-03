#pragma once

#include "Vec.h"

namespace Fastboi {
    struct Gameobject;
    struct Rect;
    class Texture;

    namespace Components {
        struct ColorComp;

        void ColorButton(Gameobject& go, const ColorComp& color);
        void TextureButton(Gameobject& go, const Texture& texture);
    }
}