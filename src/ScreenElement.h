#pragma once

#include "Transform.h"
#include "GORef.h"
#include "Vec.h"

namespace Fastboi {
    struct ScreenElement {
        GORef go;
        Position anchor = Vecf::zero();
        Size relativeSize = Vecf::zero();

        ScreenElement(GORef&& go, Position anchor, Size relSize);

        Transform GetScreenTransform() const;

    };
}