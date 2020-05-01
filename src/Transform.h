#pragma once

#include "GORef.h"
#include "Shape.h"
#include <memory>
#include <optional>
#include <type_traits>
#include "Vec.h"

namespace Fastboi {
    class GORef;

    struct Transform final {
        private:
        bool rotated = false;
        std::unique_ptr<Shape> shape;
        Transform* parent; //! I Think I'll do it like this, this is how unity does it.

        public:
        // GORef gameobject;

        Position position;
        Size size;
        double rotation = 0.f; // Degrees

        Transform();
        Transform(Position position);
        Transform(Position position, Size size, double rot);
        Transform(const Transform& copy); // Copy constructor
        Transform(Transform&& copy); // Move constructor

        ~Transform() = default;

        Transform& operator=(const Transform& copy); // Copy assignment
        Transform& operator=(Transform&& copy); // Move assignment

        double GetRotation() const;
        void SetRotation(double rot);
        void ResetRotation();

        inline bool HasParent() const { return parent != nullptr; };
        inline Transform& Parent() const { return *parent; };
        void Parent(Transform* parent);

        template<class S>
        void SetShape();

        const circular_vector<Position>& GetVertices() const;

        bool ContainsPoint(const Position& p) const;

        bool operator==(const Transform& other) const;
        bool operator!=(const Transform& other) const;

        // Parenting: Make the common case fast, so if you're a parent I will keep track of the last position, and manually update
        private:
        Position lastPosition = Position::zero();

        void UpdateLastPosRot();

        // Basically what we're going to do here is keep a master list of parent relations, as there's only ever going to be a few root parents.
        static std::vector<Transform*> rootParents;
        static void AddRootParent(Transform& rootp);
        static void ReplaceRootParent(Transform& newRoot, const Transform& oldRoot);
        static void RemoveRootParent(const Transform& rootp);
    };

    template<class S>
    void Transform::SetShape() {
        static_assert(std::is_base_of_v<Shape, S>);

        shape = std::make_unique<S>(*this);
    };
};