#pragma once

#include <compare>
#include <limits>
#include <numbers>
#include <type_traits>
#include <cstdio>
#include <cmath>

namespace Fastboi {
    using AngleNumeric_t = double;
    struct Radian;
    struct Degree;

    template<typename T>
    concept AngleType_c = (std::is_same_v<T, Degree> || std::is_same_v<T, Radian>);

    template<typename T>
    concept Arithmetic_c = std::is_arithmetic_v<T>;

    template<typename T>
    concept Radian_c = std::is_same_v<T, Radian>;

    template<typename T>
    concept Degree_c = std::is_same_v<T, Degree>;

    namespace detail {
        constexpr AngleNumeric_t floor(AngleNumeric_t x) noexcept { 
            if (x >= 0) return static_cast<int>(x);
            else return static_cast<int>(x) - 1;
        }
    }

    struct Degree {
        private:
        AngleNumeric_t value = 0;

        static constexpr AngleNumeric_t ToRadians(AngleNumeric_t deg) noexcept { return deg * std::numbers::pi_v<AngleNumeric_t> / 180.; };
        static constexpr AngleNumeric_t FromRadians(AngleNumeric_t rad) noexcept { return rad * 180. / std::numbers::pi_v<AngleNumeric_t>; };
        static constexpr AngleNumeric_t Clamp(AngleNumeric_t deg) noexcept { // Clamp between [0, 360)
            return deg - 360. * detail::floor(deg * (1. / 360.));
        };

        public:
        constexpr explicit Degree(AngleNumeric_t angle) noexcept : value(angle) { };

        template<Radian_c Radian_t>
        constexpr Degree(const Radian_t& rad) noexcept : value(FromRadians(rad.Value())) { };

        template<Radian_c Radian_t>
        constexpr Degree& operator=(const Radian_t& rad) noexcept { value = FromRadians(rad.Value()); return *this; };

        template<Arithmetic_c Num>
        constexpr explicit operator Num() const noexcept { return static_cast<Num>(value); };

        // Bounded between (-360, 360)
        template<typename As_t>
        requires (Arithmetic_c<As_t> || Degree_c<As_t> || Radian_c<As_t>)
        constexpr As_t As() const noexcept {
            if constexpr (Arithmetic_c<As_t>)
                return static_cast<As_t>(value);
            else if constexpr (Radian_c<As_t>)
                return As_t(ToRadians(value));
            else // This branch we return of type Degree
                return *this;
        };

        template<typename Cast_t>
        requires (Arithmetic_c<Cast_t> || Degree_c<Cast_t> || Radian_c<Cast_t>)
        constexpr explicit operator Cast_t() const noexcept {
            return As<Cast_t>();
        }

        template<typename As_t = AngleNumeric_t>
        requires (Arithmetic_c<As_t> || Degree_c<As_t> || Radian_c<As_t>)
        constexpr As_t operator()() const noexcept {
            return As<As_t>();
        }

        constexpr AngleNumeric_t Value() const noexcept { return value; };

        // Return Degree bounded between [0, 360)
        constexpr Degree Normalized() const noexcept { return Degree(Clamp(value)); }
        consteval static Degree Circle() noexcept { return Degree(360); };
    };
    
    struct Radian {
        private:
        AngleNumeric_t value = 0;
        static constexpr auto pi = std::numbers::pi_v<AngleNumeric_t>;


        static constexpr AngleNumeric_t ToDegrees(AngleNumeric_t rad) noexcept { return rad * 180. / pi; };
        static constexpr AngleNumeric_t FromDegrees(AngleNumeric_t deg) noexcept { return deg * pi / 180.; }
        static constexpr AngleNumeric_t Clamp(AngleNumeric_t rad) noexcept { // Clamp between (-2pi, 2pi)
                return rad - 2 * pi * detail::floor(rad * (0.5 / pi));
        };

        public:
        constexpr explicit Radian(AngleNumeric_t angle) noexcept : value(Clamp(angle)) { };
        constexpr Radian(const Degree& deg) noexcept : value(FromDegrees(deg.As<AngleNumeric_t>())) { };

        constexpr Radian& operator=(const Degree& deg) noexcept { value = FromDegrees(deg.Value()); return *this; };

        // Bounded between (-2pi, 2pi)
        template<typename As_t>
        requires (Arithmetic_c<As_t> || Degree_c<As_t> || Radian_c<As_t>)
        constexpr As_t As() const noexcept {
            if constexpr (Arithmetic_c<As_t>)
                return static_cast<As_t>(value);
            else if constexpr (Degree_c<As_t>)
                return Degree(ToDegrees(value));
            else // This branch we return of type Radian
                return *this;
        };

        template<typename Cast_t>
        requires (Arithmetic_c<Cast_t> || Degree_c<Cast_t> || Radian_c<Cast_t>)
        constexpr explicit operator Cast_t() const noexcept {
            return As<Cast_t>();
        }

        constexpr AngleNumeric_t operator()() const noexcept {
            return As<AngleNumeric_t>();
        }

        constexpr AngleNumeric_t Value() const noexcept { return value; };
        
        // Return Radian bounded between [0, 2pi)
        constexpr Radian Normalized() const noexcept { return Radian(Clamp(value)); }
        consteval static Radian PI() noexcept { return Radian(pi); };
        consteval static Radian Circle() noexcept { return Radian(2 * pi); };
    };

    constexpr Degree operator ""_deg(long double angle) { return Degree(static_cast<AngleNumeric_t>(angle)); }
    constexpr Radian operator ""_rad(long double angle) { return Radian(static_cast<AngleNumeric_t>(angle)); }
    constexpr Degree operator ""_deg(unsigned long long angle) { return Degree(static_cast<AngleNumeric_t>(angle)); }
    constexpr Radian operator ""_rad(unsigned long long angle) { return Radian(static_cast<AngleNumeric_t>(angle)); }

    template<AngleType_c Left, AngleType_c Right>
    constexpr bool operator==(const Left& left, const Right& right) {
        if constexpr (std::is_same_v<Left, Right>) {
            const auto lv = left.Normalized().Value();
            const auto rv = right.Normalized().Value();
            return abs(lv - rv) <= std::numeric_limits<AngleNumeric_t>::epsilon();
        } else
            return operator==(left, Left(right));
    }

    template<AngleType_c Left, AngleType_c Right>
    constexpr std::strong_ordering operator<=>(const Left& left, const Right& right) {
        if constexpr (std::is_same_v<Left, Right>) {
            // Normalize the values between 0 and 360
            const auto lv = left.Normalized().Value();
            const auto rv = right.Normalized().Value();
            printf("operator<=>\n");
            if (abs(lv - rv) <= std::numeric_limits<AngleNumeric_t>::epsilon())
                return std::strong_ordering::equal;
            else if (lv < rv)
                return std::strong_ordering::less;
            else
                return std::strong_ordering::greater;
        }

        return operator<=>(left, Left(right));
    }

    template<AngleType_c Angle>
    constexpr Angle operator-(const Angle& ang) {
        return Angle(-ang.Value());
    }

    #define GEN_BINARY_OP(op, opeq) \
    template<AngleType_c Left, AngleType_c Right> \
    constexpr Left& operator opeq(Left& left, const Right& right) { \
        if constexpr (std::is_same_v<Left, Right>) \
            left = Left(left.Value() op right.Value()); \
        else \
            left = Left(left.Value() op right.template As<Left>().Value()); \
 \
        return left; \
    } \
 \
    template<AngleType_c Left, AngleType_c Right> \
    constexpr Left operator op(const Left& left, const Right& right) { \
        Left tempL(left); \
        return tempL opeq right; \
    }

    #define GEN_UNARY_OP(op, opeq) \
    template<AngleType_c Angle, Arithmetic_c Num> \
    constexpr Angle& operator opeq(Angle& ang, Num s) { \
        ang = Angle(ang.Value() op (AngleNumeric_t) s); \
        return ang; \
    } \
    \
    template<AngleType_c Angle, Arithmetic_c Num> \
    constexpr Angle operator op(const Angle& ang, Num s) { \
        Angle temp(ang); \
        return temp opeq s; \
    } \
    \
    template<AngleType_c Angle, Arithmetic_c Num> \
    constexpr Angle operator op(Num s, const Angle& ang) { \
        Angle temp(ang); \
        return temp opeq s; \
    }

    GEN_BINARY_OP(+, +=)
    GEN_BINARY_OP(-, -=)
    GEN_UNARY_OP(*, *=)
    GEN_UNARY_OP(/, /=)
}
