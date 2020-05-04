#pragma once

#include "GORef.h"
#include "Shape.h"
#include <memory>
#include <optional>
#include <type_traits>
#include "Vec.h"

namespace Fastboi {
    class GORef;
    void Physics();

    struct Transform final {
        private:
        bool rotated = false;
        std::unique_ptr<Shape> shape;

        Transform* parent = nullptr;
        std::vector<Transform*> children;

        public:
        GORef gameobject;

        Position position;
        Size size;
        double rotation = 0.f; // Degrees

        Transform();
        Transform(Position position);
        Transform(Position position, Size size, double rot);
        Transform(const Transform& copy); // Copy constructor
        Transform(Transform&& copy); // Move constructor

        ~Transform();

        Transform& operator=(const Transform& copy); // Copy assignment
        Transform& operator=(Transform&& copy); // Move assignment

        double GetRotation() const;
        void SetRotation(double rot);
        void ResetRotation();

        bool HasAncestor(const Transform& check) const;
        inline bool HasParent() const { return parent != nullptr; };
        void Parent(Transform* parent);
        inline void Parent(const std::unique_ptr<Transform>& parent) { Parent(parent.get()); };
        inline void Parent(Transform& parent) { Parent(&parent); };
        inline Transform& Parent() const { return *parent; };

        inline const decltype(children)& GetChildren() const { return children; };
        bool HasChild(const Transform& check) const;

        static bool IsDescendentRelated(const Transform& a, const Transform& b);

        template<class S>
        void SetShape();

        const circular_vector<Position>& GetVertices() const;
        bool ContainsPoint(const Position& p) const;
        inline BoundingBox GetBounds() const { return shape->GetBounds(); };

        bool operator==(const Transform& other) const;
        bool operator!=(const Transform& other) const;

        // Parenting: Make the common case fast, so if you're a parent I will keep track of the last position, and manually update
        private:
        Position lastPos = Position::zero();
        double lastRot = 0;

        void AddChild(Transform& child);
        void RemoveChild(const Transform& child);
        void UpdateLastPosRot();
        void UpdateChildren(Position deltaPos, double deltaRot); // Performs a DFS on all children, updating total position changes

        // Basically what we're going to do here is keep a master list of parent relations, as there's only ever going to be a few root parents.
        static std::vector<Transform*> rootParents;
        static void AddParentStatus(Transform& rootp);
        static void RemoveParentStatus(const Transform& rootp);
        static void UpdateAllParentRelations();

        friend void Fastboi::Physics();
    };

    template<class S>
    void Transform::SetShape() {
        static_assert(std::is_base_of_v<Shape, S>);

        shape = std::make_unique<S>(*this);
    };
};