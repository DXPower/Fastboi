#include "Transform.h"
#include <algorithm>
#include <vector>

using namespace Fastboi;

Transform::Transform() : Transform(Position::zero(), Size::zero(), 0) { }
Transform::Transform(Position pos) : Transform(pos, Size::zero(), 0) { }
Transform::Transform(Position pos, Size size, double rot) : position(pos), size(size), rotation(rot), shape(nullptr) {
    SetShape<Rectangle>();
}

Transform::Transform(const Transform& copy) : Transform(copy.position, copy.size, copy.rotation) { }
Transform::Transform(Transform&& copy) : Transform(copy.position, copy.size, copy.rotation) { }

Transform::~Transform() {
    if (shape != nullptr)
        delete shape;
}

Transform& Transform::operator=(const Transform& copy) {
    position = copy.position;
    size = copy.size;
    rotation = copy.rotation;

    return *this;
}

Transform& Transform::operator=(Transform&& copy) {
    position = copy.position;
    size = copy.size;
    rotation = copy.rotation;

    return *this;
}

double Transform::GetRotation() const {
    if (!rotated) return 0.f;
    else return rotation;
}

void Transform::SetRotation(double rot) {
    rotation = rot;
    rotated = true;
}

void Transform::ResetRotation() {
    rotation = 0.f;
    rotated = false;
}

const circular_vector<Position>& Transform::GetVertices() const {
    return shape->GetVertices();
}

bool Transform::ContainsPoint(const Position& p) const {
    return shape->ContainsPoint(p);
}

bool Transform::operator==(const Transform& other) const {
    return position == other.position
        && rotation == other.rotation
        && size == other.size;
}

bool Transform::operator!=(const Transform& other) const {
    return !(*this == other);
}