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

bool Transform::HasParent(const Transform& check) const {
    Transform* p = parent;

    while (p != nullptr) {
        if (p == &check) {
            return true;
        }

        p = p->parent; //! I know why now, because I didn't give parent a default value, and that's undefined behavior in C++ :L
    }

    return false;
}

bool Transform::IsDescendentRelated(const Transform& a, const Transform& b) {
    //! Trying to think about how I can check if a is the parent/grand*n parent of b or vice versa...
    if (!a.HasParent() && !b.HasParent()) return false;

    return a.HasParent(b) || b.HasParent(a);
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
    }
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
    lastPosition = position;
    lastRot = rotation;
}

void Transform::UpdateChildren(Position deltaPos, double deltaRot) {
    //! Add deltaRot
    
    position += deltaPos;
    rotation += deltaRot;

    if (children.size() != 0) {
        deltaPos += position - lastPosition;

        for (Transform* child : children) {
            child->UpdateChildren(deltaPos, deltaRot);
        }

        UpdateLastPosRot();
    }
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