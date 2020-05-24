#include "BoundingBoxRenderer.h"
#include "Fastboi.h"

using namespace Fastboi;

void BoundingBoxRenderer::Render() {
    BoundingBox bounds = gameobject().transform->GetBounds();
    
    Transform t(gameobject().transform->position, bounds.upperBounds - bounds.lowerBounds, 0_deg);

    Rendering::SetColor(0, 255, 0, 255);
    Rendering::Render_Rect<Rendering::UNFILLED>(t);
    Rendering::SetColor(255, 0, 0, 255);

    for (auto vertex : gameobject().transform->GetVertices()) {
        Transform p(vertex, Size(10.f, 10.f), 0_deg);
        Rendering::Render_Rect<Rendering::FILLED>(p);
    }
}

BoundingBoxRenderer& BoundingBoxRenderer::Duplicate() const {
    return *(new BoundingBoxRenderer(*this));
}
