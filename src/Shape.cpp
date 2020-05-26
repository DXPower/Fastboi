#include "Shape.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include "Rect.h"
#include "Transform.h"

using namespace Fastboi;

Shape::Shape(const Transform& t) : transform(t) { }

const circular_vector<Position>& Shape::GetVertices() const {
    if (   lastPosVerts      != transform.position
        || lastSizeVerts     != transform.size
        || lastRotationVerts != transform.rotation
    ) {
        lastPosVerts = transform.position;
        lastSizeVerts = transform.size;
        lastRotationVerts = transform.rotation;

        vertices = CalculateVertices();
    }

    return vertices;
}

BoundingBox Shape::GetBounds() const {
    if (   lastPosBounds      != transform.position
        || lastSizeBounds     != transform.size
        || lastRotationBounds != transform.rotation
    ) {
        lastPosBounds = transform.position;
        lastSizeBounds = transform.size;
        lastRotationBounds = transform.rotation;

        bounds = CalculateBounds();
    }

    return bounds;
}

BoundingBox Shape::CalculateBounds() const {
    using nlf = std::numeric_limits<float>;

    BoundingBox bounds{ .lowerBounds=Vecf(nlf::max(), nlf::max()), .upperBounds=Vecf(nlf::lowest(), nlf::lowest()) };

    auto verts = GetVertices();

    for (auto vertex : verts) {
        bounds.lowerBounds.x = std::min(vertex.x, bounds.lowerBounds.x);
        bounds.lowerBounds.y = std::min(vertex.y, bounds.lowerBounds.y);
        
        bounds.upperBounds.x = std::max(vertex.x, bounds.upperBounds.x);
        bounds.upperBounds.y = std::max(vertex.y, bounds.upperBounds.y);
    }

    // printf("Bounds === lowest: %f %f, highest: %f %f\n", bounds.lowerBounds.x, bounds.lowerBounds.y, bounds.upperBounds.x, bounds.upperBounds.y);

    return bounds;
}

// Gets what "side" of a line a point is on. 1 means "left", -1 means "right", 0 means on the line.
int_fast8_t GetPointSide(const Position& a, const Position& b, const Position& p) {
    float dir = (p.x - a.x) * (b.y - a.y) - (p.y - a.y) * (b.x - a.x);

    if (abs(dir) > std::numeric_limits<float>::epsilon())
        if (std::signbit(dir))
            return -1;
        else
            return 1;
    else
        return 0;
}

// Implements the edge winding algorithm
bool Shape::ContainsPoint(const Position& p) {
    const circular_vector<Position>& verts = GetVertices();

    int_fast8_t windingSide = 2; // 2 is an invalid value for GetPointSide

    for (auto pit = verts.pair_begin(); pit != verts.pair_end(); pit++) {
        int_fast8_t side = GetPointSide(*pit.first, *pit.second, p);

        if (side == 0) return true; // The point is on the line, so thus it is contained

        if (pit != verts.pair_begin()) {
            if (side != windingSide) // If the winding direction changes, it is not contained
                return false;
        } else {
            windingSide = side; // Sets the winding direction on the first edge
        }
    }

    return true;
}

bool BoundingBox::ContainsPoint(const Position& pos) const {
    return pos.x >= lowerBounds.x && pos.y >= lowerBounds.y
        && pos.x >= upperBounds.x && pos.y >= upperBounds.y;
}

bool BoundingBox::Overlaps(const BoundingBox& o) const {
    const Vecf aL = lowerBounds, aU = upperBounds;
    const Vecf bL = o.lowerBounds, bU = o.upperBounds;

    if (aU.x < bL.x || bU.x < aL.x) return false;
    if (aU.y < bL.y || bU.y < aL.y) return false;

    return true;
}

BoundingBox BoundingBox::Fatten(float factor) const {
    const Size deltaSize = (upperBounds - lowerBounds) * factor;

    return { .lowerBounds = lowerBounds - deltaSize, .upperBounds = upperBounds + deltaSize };
}

RectF BoundingBox::ToRect() const {
    const Size size = upperBounds - lowerBounds;
    return RectF(lowerBounds.x, lowerBounds.y, size.x, size.y);
}

Rectangle::Rectangle(const Transform& t) : Shape(t) {
    GetVertices();
    GetBounds();
}

circular_vector<Position> Rectangle::CalculateVertices() const {
    const Size& size = transform.size;
    const Position& pos = transform.position;
    const double rads = transform.rotation.As<Radian>().Value();

    circular_vector<Position> verts = circular_vector<Position>();

    // Get corners in a coordinate space where the center of the rect is the origin
    verts.push_back( size / 2.f );
    verts.push_back( size / Vecf(-2.f, 2.f) );
    verts.push_back( size / -2.f);
    verts.push_back( size / Vecf(2.f, -2.f) );

    // Map the rotation function to every point
    std::for_each(verts.begin(), verts.end(), 
        [&](Position& p) {
            const float rx = (p.x * cos(rads)) - (p.y * sin(rads)); // Rotated x val
            const float ry = (p.x * sin(rads)) + (p.y * cos(rads)); // Rotated y val

            p = Position(rx, ry) + pos; // Translate back to global coorindate system
        }
    );

    return verts;
}