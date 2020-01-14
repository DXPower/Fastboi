#pragma once

#include <cmath>
#include <stdint.h>
#include <utility>
#include <type_traits>
#include <limits>
#include <cmath>

template<class T>
struct Vec {
    T x;
    T y;

    constexpr Vec() : Vec(T(0), T(0)) { }; // Default constructor
    constexpr Vec(T x, T y) : x(x), y(y) { };  // Constructor
    constexpr Vec(const Vec<T>& v) : x(v.x), y(v.y) { }; // Copy constructor
    constexpr Vec(Vec<T>&& v) noexcept : x(v.x), y(v.y) { }; // Move constructor

    // Copy assignment operator
    Vec<T>& operator=(const Vec<T>& v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    // Move assignment operator
    Vec<T>& operator=(Vec<T>&& v) noexcept {
        std::swap(x, v.x);
        std::swap(y, v.y);
        return *this;
    }

    T magnitude2() const {
        return (x * x) + (y * y);
    }

    T magnitude() const {
        return sqrt(magnitude2());
    }
    
    Vec<T> normalized() const {
        if (x == 0 && y == 0) return Vec<T>::zero();
        return Vec<T>(x / magnitude(), y / magnitude());
    }

    // Vector|Vector operators
    Vec<T>& operator+=(const Vec<T>& v) { x += v.x; y += v.y; return *this; };
    Vec<T>& operator-=(const Vec<T>& v) { x -= v.x; y -= v.y; return *this; };
    Vec<T>& operator*=(const Vec<T>& v) { x *= v.x; y *= v.y; return *this; };
    Vec<T>& operator/=(const Vec<T>& v) { x /= v.x; y /= v.y; return *this; };

    // Vector|Scalar operators
    Vec<T>& operator+=(const T& s) { x += s; y += s; return *this; };
    Vec<T>& operator-=(const T& s) { x -= s; y -= s; return *this; };
    Vec<T>& operator*=(const T& s) { x *= s; y *= s; return *this; };
    Vec<T>& operator/=(const T& s) { x /= s; y /= s; return *this; };

    // Negation operators
    constexpr Vec<T> operator-() const { return Vec<T>(-x, -y); };
    Vec<T>& negate() { *this *= -1; return *this; };

    friend bool operator!=(const Vec<T>& lhs, const Vec<T>& rhs) { return !(lhs == rhs); };
    friend bool operator==(const Vec<T>& lhs, const Vec<T>& rhs) { 
        if constexpr (!std::is_floating_point_v<T>)
            return lhs.x == rhs.x && lhs.y == rhs.y;
        else
            return ((T) abs(lhs.x - rhs.x)) <= std::numeric_limits<T>::epsilon()
                && ((T) abs(lhs.y - rhs.y)) <= std::numeric_limits<T>::epsilon();
    };

    // Non-assignment math operators
    friend Vec<T> operator+(const Vec<T>& lhs, const Vec<T>& rhs) { return Vec<T>(lhs) += rhs; };
    friend Vec<T> operator-(const Vec<T>& lhs, const Vec<T>& rhs) { return Vec<T>(lhs) -= rhs; };
    friend Vec<T> operator*(const Vec<T>& lhs, const Vec<T>& rhs) { return Vec<T>(lhs) *= rhs; };
    friend Vec<T> operator/(const Vec<T>& lhs, const Vec<T>& rhs) { return Vec<T>(lhs) /= rhs; };

    friend Vec<T> operator+(const Vec<T>& lhs, const T& s) { return Vec<T>(lhs) += s; };
    friend Vec<T> operator+(const T& s, const Vec<T>& rhs) { return Vec<T>(rhs) += s; };
    friend Vec<T> operator-(const Vec<T>& lhs, const T& s) { return Vec<T>(lhs) -= s; };
    friend Vec<T> operator*(const Vec<T>& lhs, const T& s) { return Vec<T>(lhs) *= s; };
    friend Vec<T> operator*(const T& s, const Vec<T>& rhs) { return Vec<T>(rhs) *= s; };
    friend Vec<T> operator/(const Vec<T>& lhs, const T& s) { return Vec<T>(lhs) /= s; };

    constexpr static Vec<T> zero() {
        return Vec<T>(0, 0);
    }

    // Cast
    template<typename Y>
    operator Vec<Y>() const {
        return Vec<Y>(Y(x), Y(y));
    }

    // Swap
    void swap(Vec<T>& rhs) {
        std::swap(x, rhs.x);
        std::swap(y, rhs.y);
    }

    static T dotProduct(const Vec<T>& lhs, const Vec<T>& rhs) {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y);
    }

    static Vec<T> tripleProduct(const Vec<T>& a, const Vec<T>& b, const Vec<T>& c) {
        return (b * dotProduct(a, c)) - (a * dotProduct(b, c));
    }
};



namespace std {
    template<class T>
    inline void swap(Vec<T>& lhs, Vec<T>& rhs) {
        lhs.swap(rhs);
    }
};

using Vecl = Vec<uint64_t>;
using Veci = Vec<uint32_t>;
using Vecs = Vec<uint16_t>;
using Vecb = Vec<uint8_t>;
using Vecf = Vec<float_t>;
using Vecd = Vec<double_t>;

using Position = Vecf;
using Velocity = Vecf;
using Size = Vecf;