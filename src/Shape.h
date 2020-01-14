#pragma once

#include "circular_vector.h"
#include "Vec.h"

struct Transform;

class Shape {
    protected:
    circular_vector<Position> vertices;

    // Store the last known position, size, and rotation so we only have to recalculate vertices when they are changed
    Position lastPos;
    Size lastSize;
    double lastRotation;

    const Transform& transform;

    public:
    //! Does not calculate vertices. Must be done by derived class (due to C++ polymorphistic restrictions)
    Shape(const Transform& transform);
    virtual ~Shape() { };

    const circular_vector<Position>& GetVertices();
    virtual circular_vector<Position> CalculateVertices() const = 0;

    bool ContainsPoint(const Position& p);
};

class Rectangle final : public Shape {
    public:

    Rectangle(const Transform& transform);
    ~Rectangle() { };

    protected:
    circular_vector<Position> CalculateVertices() const override;
};