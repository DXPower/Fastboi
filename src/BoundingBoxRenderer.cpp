#include "BoundingBoxRenderer.h"
#include "Fastboi.h"

using namespace Fastboi;

void BoundingBoxRenderer::Render() {
    BoundingBox bounds = gameobject().transform->GetBounds();
    Transform t((bounds.upperBounds + bounds.lowerBounds) * 0.5, (bounds.upperBounds - bounds.lowerBounds) / 2, 0);

    Rendering::Render_Rect<Rendering::UNFILLED>(t);
}

BoundingBoxRenderer& BoundingBoxRenderer::Duplicate() const {
    return *(new BoundingBoxRenderer(*this));
}
