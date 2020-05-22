#pragma once

#include <type_traits>

#define _USE_MATH_DEFINES
#include <math.h>
#undef _USE_MATH_DEFINES

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

    struct Degree {
        private:
        AngleNumeric_t value;

        static constexpr AngleNumeric_t ToRadians(AngleNumeric_t deg) noexcept { return deg * M_PI / 180.; };
        static constexpr AngleNumeric_t FromRadians(AngleNumeric_t rad) noexcept { return rad * 180. / M_PI; };

        public:
        constexpr explicit Degree(AngleNumeric_t angle) noexcept : value(angle) { };

        template<Radian_c Radian_t>
        constexpr explicit Degree(const Radian_t& rad) noexcept : value(FromRadians(rad.Value())) { };

        template<Radian_c Radian_t>
        constexpr Degree& operator=(const Radian_t& rad) noexcept { value = FromRadians(rad.Value()); return *this; };

        template<Arithmetic_c Num>
        constexpr explicit operator Num() const noexcept { return static_cast<Num>(value); };

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

        constexpr AngleNumeric_t Value() const noexcept { return value; };
    };
    
    struct Radian {
        private:
        AngleNumeric_t value;

        static constexpr AngleNumeric_t ToDegrees(AngleNumeric_t rad) noexcept { return rad * 180. / M_PI; };
        static constexpr AngleNumeric_t FromDegrees(AngleNumeric_t deg) noexcept { return deg * M_PI / 180.; }

        public:
        constexpr explicit Radian(AngleNumeric_t angle) noexcept : value(angle) { };
        constexpr explicit Radian(const Degree& deg) noexcept : value(FromDegrees(deg.As<AngleNumeric_t>())) { };

        constexpr Radian& operator=(const Degree& deg) noexcept { value = FromDegrees(deg.Value()); return *this; };

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
        
        constexpr AngleNumeric_t Value() const noexcept { return value; };
        constexpr static Radian PI() noexcept { return Radian(M_PI); };
    };

    constexpr Degree operator ""_deg(long double angle) { return Degree(static_cast<AngleNumeric_t>(angle)); }
    constexpr Radian operator ""_rad(long double angle) { return Radian(static_cast<AngleNumeric_t>(angle)); }
    constexpr Degree operator ""_deg(unsigned long long angle) { return Degree(static_cast<AngleNumeric_t>(angle)); }
    constexpr Radian operator ""_rad(unsigned long long angle) { return Radian(static_cast<AngleNumeric_t>(angle)); }

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
    constexpr Left operator op(const Left&& left, const Right&& right) { \
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
    constexpr Angle operator op(const Angle&& ang, Num s) { \
        Angle temp(ang); \
        return temp opeq s; \
    } \
    \
    template<AngleType_c Angle, Arithmetic_c Num> \
    constexpr Angle operator op(Num s, const Angle&& ang) { \
        Angle temp(ang); \
        return temp opeq s; \
    }

    GEN_BINARY_OP(+, +=)
    GEN_BINARY_OP(-, -=)
    GEN_UNARY_OP(*, *=)
    GEN_UNARY_OP(/, /=)
}
