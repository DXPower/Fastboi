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

        constexpr Vec() = default;
        constexpr Vec(T x, T y) : x(x), y(y) { }

        constexpr T magnitude2() const noexcept {
            return (x * x) + (y * y);
        }

        constexpr T magnitude() const noexcept {
            return sqrt(magnitude2());
        }
        
        constexpr Vec<T> normalized() const noexcept {
            if (x == 0 && y == 0) return Vec<T>::zero();
            return Vec<T>(x / magnitude(), y / magnitude());
        }

        // Returns a vector that is perpendicular to this
        template<int = 0>
        requires std::is_signed_v<T>
        constexpr Vec<T> ortho() const noexcept {
            return Vec<T>(this->y, -this->x);
        }

        // Vector|Vector operators
        constexpr Vec<T>& operator+=(const Vec<T>& v) noexcept { x += v.x; y += v.y; return *this; };
        constexpr Vec<T>& operator-=(const Vec<T>& v) noexcept { x -= v.x; y -= v.y; return *this; };
        constexpr Vec<T>& operator*=(const Vec<T>& v) noexcept { x *= v.x; y *= v.y; return *this; };
        constexpr Vec<T>& operator/=(const Vec<T>& v) { x /= v.x; y /= v.y; return *this; };

        // Vector|Scalar operators
        constexpr Vec<T>& operator+=(const T& s) noexcept { x += s; y += s; return *this; };
        constexpr Vec<T>& operator-=(const T& s) noexcept { x -= s; y -= s; return *this; };
        constexpr Vec<T>& operator*=(const T& s) noexcept { x *= s; y *= s; return *this; };
        constexpr Vec<T>& operator/=(const T& s) { x /= s; y /= s; return *this; };

        // Negation operators
        template<int = 0>
        requires std::is_signed_v<T>
        constexpr Vec<T> operator-() const noexcept { return Vec<T>(-x, -y); };
        
        template<int = 0>
        requires std::is_signed_v<T>
        Vec<T>& negate() noexcept { *this *= -1; return *this; };

        friend constexpr bool operator!=(const Vec<T>& lhs, const Vec<T>& rhs) noexcept { return !(lhs == rhs); };
        friend constexpr bool operator==(const Vec<T>& lhs, const Vec<T>& rhs) noexcept { 
            if constexpr (!std::is_floating_point_v<T>)
                return lhs.x == rhs.x && lhs.y == rhs.y;
            else
                return ((T) abs(lhs.x - rhs.x)) <= std::numeric_limits<T>::epsilon()
                    && ((T) abs(lhs.y - rhs.y)) <= std::numeric_limits<T>::epsilon();
        };

        // Non-assignment math operators
        friend constexpr Vec<T> operator+(const Vec<T>& lhs, const Vec<T>& rhs) noexcept { return Vec<T>(lhs) += rhs; };
        friend constexpr Vec<T> operator-(const Vec<T>& lhs, const Vec<T>& rhs) noexcept { return Vec<T>(lhs) -= rhs; };
        friend constexpr Vec<T> operator*(const Vec<T>& lhs, const Vec<T>& rhs) noexcept { return Vec<T>(lhs) *= rhs; };
        friend constexpr Vec<T> operator/(const Vec<T>& lhs, const Vec<T>& rhs) { return Vec<T>(lhs) /= rhs; };

        friend constexpr Vec<T> operator+(const Vec<T>& lhs, const T& s) noexcept { return Vec<T>(lhs) += s; };
        friend constexpr Vec<T> operator+(const T& s, const Vec<T>& rhs) noexcept { return Vec<T>(rhs) += s; };
        friend constexpr Vec<T> operator-(const Vec<T>& lhs, const T& s) noexcept { return Vec<T>(lhs) -= s; };
        friend constexpr Vec<T> operator*(const Vec<T>& lhs, const T& s) noexcept { return Vec<T>(lhs) *= s; };
        friend constexpr Vec<T> operator*(const T& s, const Vec<T>& rhs) noexcept { return Vec<T>(rhs) *= s; };
        friend constexpr Vec<T> operator/(const Vec<T>& lhs, const T& s) { return Vec<T>(lhs) /= s; };

        constexpr static Vec<T> zero() noexcept {
            return Vec<T>(0, 0);
        }

        // Cast
        template<typename Y>
        constexpr operator Vec<Y>() const noexcept {
            return Vec<Y>(Y(x), Y(y));
        }

        // Swap
        void swap(Vec<T>& rhs) noexcept {
            std::swap(x, rhs.x);
            std::swap(y, rhs.y);
        }

        static constexpr T dotProduct(const Vec<T>& lhs, const Vec<T>& rhs) noexcept {
            return (lhs.x * rhs.x) + (lhs.y * rhs.y);
        }

        static constexpr Vec<T> tripleProduct(const Vec<T>& a, const Vec<T>& b, const Vec<T>& c) noexcept {
            return (b * dotProduct(a, c)) - (a * dotProduct(b, c));
        }

        static constexpr Vec<T> projection(const Vec<T>& vec, const Vec<T>& line) {
            return (dotProduct(vec, line) / dotProduct(line, line)) * line;
        }

        static constexpr Vec<T> minTransform(const Vec<T>& a, const Vec<T>& b) noexcept {
            return { std::min(a.x, b.x), std::min(a.y, b.y) };
        }

        static constexpr Vec<T> maxTransform(const Vec<T>& a, const Vec<T>& b) noexcept {
            return { std::max(a.x, b.x), std::max(a.y, b.y) };
        }
    };

    template<class T>
    inline void swap(Vec<T>& lhs, Vec<T>& rhs) noexcept {
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