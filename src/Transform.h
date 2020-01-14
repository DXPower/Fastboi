#pragma once

#include "Vec.h"
#include "Shape.h"
#include <type_traits>

struct Transform {
    private:
    bool rotated = false;
    Shape* shape;

    public:
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

    template<class S>
    void SetShape();

    const circular_vector<Position>& GetVertices() const;

    bool ContainsPoint(const Position& p) const;

    bool operator==(const Transform& other) const;
    bool operator!=(const Transform& other) const;
};

template<class S>
void Transform::SetShape() {
    static_assert(std::is_base_of_v<Shape, S>);

    if (shape != nullptr)
        delete shape;

    shape = new S(*this);
}