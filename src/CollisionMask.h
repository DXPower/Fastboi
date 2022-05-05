#pragma once

#include <concepts>
#include <cstdint>
#include <type_traits>
#include "Application.h"

namespace Fastboi {
    enum class CollisionLayer {
        ALL = 1,
        WALLS = 2,
        PLAYER = 4,
        UNITS = 8,
        ITEMS = 16,
        NONE = 32,
        TRIGGERS = 64,
        PARTICLES = 128
    };

    struct CollisionMask {
        private:
        using UT = std::underlying_type_t<CollisionLayer>;
        #define stut(x) static_cast<UT>(x)

        UT inclusions = stut(CollisionLayer::ALL);

        public:
        CollisionLayer layer;
        
        constexpr CollisionMask(CollisionLayer layer) : layer(layer) {
            if (layer == CollisionLayer::ALL || layer == CollisionLayer::NONE)
                Application::ThrowRuntimeException("CollisionMask layer can't be ALL or NONE.", Application::INVALID_COL_LAYER);
        };

        template<typename... EnumClass>
        requires std::same_as<std::common_type_t<EnumClass...>, CollisionLayer>
        CollisionMask& Include(EnumClass... layers) {
            if (((layers == CollisionLayer::ALL) || ...))
                inclusions = stut(CollisionLayer::ALL);
            else if (((layers == CollisionLayer::NONE) || ...))
                inclusions = stut(CollisionLayer::NONE);
            else {
                UT merged = (static_cast<UT>(layers) | ...);
                
                if (inclusions == stut(CollisionLayer::ALL))
                    inclusions = merged;
                else
                    inclusions |= merged;
            };

            return *this;
        };

        template<typename... EnumClass>
        requires std::same_as<std::common_type_t<EnumClass...>, CollisionLayer>
        CollisionMask& Exclude(EnumClass... layers) {
            if (inclusions == stut(CollisionLayer::NONE)) return *this;

            using CT = std::common_type_t<EnumClass...>;

            if (((layers == CollisionLayer::ALL) || ...))
                inclusions = stut(CollisionLayer::NONE);
            else if (((layers == CollisionLayer::NONE) || ...))
                inclusions = stut(CollisionLayer::ALL);
            else {
                UT exclusions = (static_cast<UT>(layers) | ...);
                UT result;

                exclusions = ~exclusions; // Flip all excluded bits to 0

                if (inclusions == CollisionLayer::ALL)
                    result = ~static_cast<UT>(0x0); // Set all bits to 1 if the inclusions is ALL
                else
                    result = inclusions;

                inclusions = result & exclusions;
            }

            return *this;
        }

        CollisionMask& Clear() {
            inclusions = stut(CollisionLayer::NONE);

            return *this;
        }

        static bool CanCollide(const CollisionMask& a, const CollisionMask& b);

        private:
        bool CanCollide(CollisionLayer layer) const;

        #undef stut
    };
}