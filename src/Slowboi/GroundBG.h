#pragma once

#include <initializer_list>
#include "Fastboi.h"

using namespace Fastboi;

namespace Slowboi {
    void GroundBG(Gameobject& go, const Position& pos, const Size& size, const Texture& spritesheet,  const Size& tileSize, const std::vector<Rect>& sprites);
}