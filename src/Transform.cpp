#include "Transform.h"
#include <algorithm>
#include <vector>

using namespace Fastboi;

std::vector<Transform*> Transform::rootParents = std::vector<Transform*>();

Transform::Transform() : Transform(Position::zero(), Size::zero(), 0) { }
Transform::Transform(Position pos) : Transform(pos, Size::zero(), 0) { }
Transform::Transform(Position pos, Size size, double rot) : position(pos), size(size), rotation(rot), shape(nullptr) {
    SetShape<Rectangle>();
}

Transform::Transform(const Transform& copy) : Transform(copy.position, copy.size, copy.rotation) { }
Transform::Transform(Transform&& copy) : Transform(copy.position, copy.size, copy.rotation) { }

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

void Transform::Parent(Transform* parent) {
    if (parent != nullptr) {
        this->parent = parent;
        AddRootParent(*this->parent);
    } else {

    }
}

void Transform::UpdateLastPosRot() {
    lastPosition = position;
    //! Add lastRot here
}

void Transform::AddRootParent(Transform& rootp) {
    rootp.UpdateLastPosRot();
    rootParents.push_back(&rootp);
}

void Transform::ReplaceRootParent(Transform& newRoot, const Transform& oldRoot) {
    for (auto it = rootParents.begin(); it != rootParents.end(); it++) {
        if (*it == &oldRoot) {
            *it = &newRoot;
            break;
        }
    }
}

void Transform::RemoveRootParent(const Transform& rootp) {
    for (auto it = rootParents.begin(); it != rootParents.end(); it++) {
        if (*it == &rootp) {
            rootParents.erase(it);
            break;
        }
    }
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