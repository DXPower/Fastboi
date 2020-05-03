#include "Transform.h"
#include <algorithm>
#include <vector>

using namespace Fastboi;

std::vector<Transform*> Transform::rootParents = std::vector<Transform*>();

Transform::Transform() : Transform(Position::zero(), Size::zero(), 0) { }
Transform::Transform(Position pos) : Transform(pos, Size::zero(), 0) { }
Transform::Transform(Position pos, Size size, double rot) : position(pos), size(size), rotation(rot), shape(nullptr) {
    SetRotation(rot);
    SetShape<Rectangle>();
}

Transform::Transform(const Transform& copy) : Transform(copy.position, copy.size, copy.rotation) { }
Transform::Transform(Transform&& copy) : Transform(copy.position, copy.size, copy.rotation) { }

Transform::~Transform() {
    Parent(nullptr); // Reset our parenthood to update the parent transform

    // Remove child status from all our children
    for (Transform* c : children) {
        c->Parent(nullptr);
    }
}

Transform& Transform::operator=(const Transform& copy) {
    position = copy.position;
    size = copy.size;
    rotation = copy.rotation;

    SetShape<Rectangle>();

    return *this;
}

Transform& Transform::operator=(Transform&& copy) {
    position = copy.position;
    size = copy.size;
    rotation = copy.rotation;

    SetShape<Rectangle>();

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

bool Transform::HasAncestor(const Transform& check) const {
    Transform* p = parent;

    while (p != nullptr) {
        if (p == &check) {
            return true;
        }

        p = p->parent;
    }

    return false;
}

bool Transform::IsDescendentRelated(const Transform& a, const Transform& b) {
    if (!a.HasParent() && !b.HasParent()) return false;

    return a.HasAncestor(b) || b.HasAncestor(a);
}


void Transform::Parent(Transform* p) {
    if (parent == p) return; // If we're setting the same parent, do nothing.

    if (p != nullptr) {
        // If we already have a parent, make sure to remove us from their children and update their parent status
        if (parent != nullptr) {
            parent->RemoveChild(*this);
            RemoveParentStatus(*parent);
        }

        // Now, add us to our new parent's children and update their parent status
        parent = p;
        parent->AddChild(*this);
        AddParentStatus(*parent);
    } else {
        // Remove us from our parent's children and update their parent status
        RemoveParentStatus(*parent);
        parent->RemoveChild(*this);
        this->parent = nullptr;

        AddParentStatus(*this); // If we were previously a child but are now a root parent, this is what we do
    }
}

bool Transform::HasChild(const Transform& check) const {
    return std::find(children.begin(), children.end(), &check) != children.end();
}

void Transform::AddChild(Transform& child) {
    children.push_back(&child);
}

void Transform::RemoveChild(const Transform& child) {
    for (auto it = children.begin(); it != children.end(); it++) {
        children.erase(it);
    }
}

void Transform::UpdateLastPosRot() {
    lastPos = position;
    lastRot = rotation;
}

#define PI 3.14159265358979323846
#define TO_RADIANS(x) x * PI / 180.0

void Transform::UpdateChildren(Position deltaPos, double deltaRot) {
    // Calculate change in position due to parent's rotation
    if (deltaRot != 0) {
        Position rotPos = position - parent->position; // Transform to coordinates of parent
        double dRotRads = TO_RADIANS(deltaRot);

        const float rx = (rotPos.x * cos(dRotRads)) - (rotPos.y * sin(dRotRads)); // Rotated x val
        const float ry = (rotPos.x * sin(dRotRads)) + (rotPos.y * cos(dRotRads)); // Rotated y val

        deltaPos += Position(rx, ry) + parent->position - position; // Get deltaPos in global coordinate system
    }

    // Calculate change in position/rotation for children if and only if we have children
    if (children.size() != 0) {
        Position childDeltaPos = deltaPos + position - lastPos; // We need to add the parent's deltaPos as well as our own deltaPos
        double childDeltaRot = deltaRot + rotation - lastRot; // We need to add the parent's deltaRot as well as our own deltaRot

        for (Transform* child : children) {
            child->UpdateChildren(childDeltaPos, childDeltaRot); // Apply the new deltaPos/Rot for each child
        }

    }
    
    // Save our new position from the propogation
    position += deltaPos;
    SetRotation(rotation + deltaRot);

    // We only track last pos/rot if we have children
    if (children.size() != 0)
        UpdateLastPosRot();
}

void Transform::AddParentStatus(Transform& rootp) {
    rootp.UpdateLastPosRot();

    // If the transform has more than 1 child, then it is already in rootParents. If it has a parent, then it is not a root so shouldn't be added.
    if (rootp.children.size() == 1 && !rootp.HasParent()) {
        rootParents.push_back(&rootp);
    }
}

void Transform::RemoveParentStatus(const Transform& rootp) {
    // Check if rootp has other children or is a child itself, then we shouldn't remove the "rootness" of it
    // (if it has a parent, then it can't be a root so it won't be in the vector)
    if (rootp.HasParent() || rootp.children.size() != 0)
        return;

    for (auto it = rootParents.begin(); it != rootParents.end(); it++) {
        if (*it == &rootp) {
            rootParents.erase(it);
            break;
        }
    }
}

void Transform::UpdateAllParentRelations() {
    for (Transform* t : rootParents) {
        t->UpdateChildren(Position::zero(), 0.0); // Does a DFS of the tree of parenthood for this root parent
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