#pragma once

#include "circular_vector.h"
#include "Vec.h"

namespace Fastboi {
    struct Transform;

    struct BoundingBox {
        Vecf lowerBounds; // Lower bounds in x and y dirs
        Vecf upperBounds; // Upper bounds in x and y dirs
    };

    class Shape {
        protected:
        mutable circular_vector<Position> vertices;
        mutable BoundingBox bounds;

        // Store the last known position, size, and rotation so we only have to recalculate vertices when they are changed
        mutable Position lastPosVerts;
        mutable Size lastSizeVerts;
        mutable double lastRotationVerts;

        // Store last known pos, size, and rots for bounds calcs
        mutable Position lastPosBounds;
        mutable Size lastSizeBounds;
        mutable double lastRotationBounds;

        const Transform& transform;

        public:
        //! Does not calculate vertices. Must be done by derived class (due to C++ polymorphistic restrictions)
        Shape(const Transform& transform);
        virtual ~Shape() { };

        const circular_vector<Position>& GetVertices() const; // Lazy: Vertices are calculated/retrieved from cache on request
        BoundingBox GetBounds() const; // Lazy: Bounds are calculated/retrieved from cache on request

        bool ContainsPoint(const Position& p);

        protected:
        virtual BoundingBox CalculateBounds() const;
        virtual circular_vector<Position> CalculateVertices() const = 0;
    };

    class Rectangle final : public Shape {
        public:

        Rectangle(const Transform& transform);
        ~Rectangle() { };

        protected:
        circular_vector<Position> CalculateVertices() const override;
    };
};