#pragma once

#include <cmath>
#include <stdint.h>
#include <utility>
#include <type_traits>
#include <limits>
#include <cmath>

namespace Fastboi {
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

        constexpr T magnitude2() const {
            return (x * x) + (y * y);
        }

        constexpr T magnitude() const {
            return sqrt(magnitude2());
        }
        
        constexpr Vec<T> normalized() const {
            if (x == 0 && y == 0) return Vec<T>::zero();
            return Vec<T>(x / magnitude(), y / magnitude());
        }

        // Returns a vector that is perpendicular to this
        template<int = 0>
        requires std::is_signed_v<T>
        constexpr Vec<T> ortho() const {
            return Vec<T>(this->y, -this->x);
        }

        // Vector|Vector operators
        constexpr Vec<T>& operator+=(const Vec<T>& v) { x += v.x; y += v.y; return *this; };
        constexpr Vec<T>& operator-=(const Vec<T>& v) { x -= v.x; y -= v.y; return *this; };
        constexpr Vec<T>& operator*=(const Vec<T>& v) { x *= v.x; y *= v.y; return *this; };
        constexpr Vec<T>& operator/=(const Vec<T>& v) { x /= v.x; y /= v.y; return *this; };

        // Vector|Scalar operators
        constexpr Vec<T>& operator+=(const T& s) { x += s; y += s; return *this; };
        constexpr Vec<T>& operator-=(const T& s) { x -= s; y -= s; return *this; };
        constexpr Vec<T>& operator*=(const T& s) { x *= s; y *= s; return *this; };
        constexpr Vec<T>& operator/=(const T& s) { x /= s; y /= s; return *this; };

        // Negation operators
        template<int = 0>
        requires std::is_signed_v<T>
        constexpr Vec<T> operator-() const { return Vec<T>(-x, -y); };
        
        template<int = 0>
        requires std::is_signed_v<T>
        Vec<T>& negate() { *this *= -1; return *this; };

        friend constexpr bool operator!=(const Vec<T>& lhs, const Vec<T>& rhs) { return !(lhs == rhs); };
        friend constexpr bool operator==(const Vec<T>& lhs, const Vec<T>& rhs) { 
            if constexpr (!std::is_floating_point_v<T>)
                return lhs.x == rhs.x && lhs.y == rhs.y;
            else
                return ((T) abs(lhs.x - rhs.x)) <= std::numeric_limits<T>::epsilon()
                    && ((T) abs(lhs.y - rhs.y)) <= std::numeric_limits<T>::epsilon();
        };

        // Non-assignment math operators
        friend constexpr Vec<T> operator+(const Vec<T>& lhs, const Vec<T>& rhs) { return Vec<T>(lhs) += rhs; };
        friend constexpr Vec<T> operator-(const Vec<T>& lhs, const Vec<T>& rhs) { return Vec<T>(lhs) -= rhs; };
        friend constexpr Vec<T> operator*(const Vec<T>& lhs, const Vec<T>& rhs) { return Vec<T>(lhs) *= rhs; };
        friend constexpr Vec<T> operator/(const Vec<T>& lhs, const Vec<T>& rhs) { return Vec<T>(lhs) /= rhs; };

        friend constexpr Vec<T> operator+(const Vec<T>& lhs, const T& s) { return Vec<T>(lhs) += s; };
        friend constexpr Vec<T> operator+(const T& s, const Vec<T>& rhs) { return Vec<T>(rhs) += s; };
        friend constexpr Vec<T> operator-(const Vec<T>& lhs, const T& s) { return Vec<T>(lhs) -= s; };
        friend constexpr Vec<T> operator*(const Vec<T>& lhs, const T& s) { return Vec<T>(lhs) *= s; };
        friend constexpr Vec<T> operator*(const T& s, const Vec<T>& rhs) { return Vec<T>(rhs) *= s; };
        friend constexpr Vec<T> operator/(const Vec<T>& lhs, const T& s) { return Vec<T>(lhs) /= s; };

        constexpr static Vec<T> zero() {
            return Vec<T>(0, 0);
        }

        // Cast
        template<typename Y>
        constexpr operator Vec<Y>() const {
            return Vec<Y>(Y(x), Y(y));
        }

        // Swap
        void swap(Vec<T>& rhs) {
            std::swap(x, rhs.x);
            std::swap(y, rhs.y);
        }

        static constexpr T dotProduct(const Vec<T>& lhs, const Vec<T>& rhs) {
            return (lhs.x * rhs.x) + (lhs.y * rhs.y);
        }

        static constexpr Vec<T> tripleProduct(const Vec<T>& a, const Vec<T>& b, const Vec<T>& c) {
            return (b * dotProduct(a, c)) - (a * dotProduct(b, c));
        }

        static constexpr Vec<T> projection(const Vec<T>& vec, const Vec<T>& line) {
            return (dotProduct(vec, line) / dotProduct(line, line)) * line;
        }
    };

    template<class T>
    inline void swap(Vec<T>& lhs, Vec<T>& rhs) {
        lhs.swap(rhs);
    }

    using Vecl = Vec<uint64_t>;
    using Veci = Vec<uint32_t>;
    using Vecs = Vec<uint16_t>;
    using Vecb = Vec<uint8_t>;
    using Vecf = Vec<float_t>;
    using Vecd = Vec<double_t>;

    using Position = Vecf;
    using Velocity = Vecf;
    using Size = Vecf;
};