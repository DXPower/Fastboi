#pragma once

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
        TRIGGERS = 64
    };

    struct CollisionMask {
        private:
        CollisionLayer inclusions = CollisionLayer::ALL;

        public:
        CollisionLayer layer;
        
        constexpr CollisionMask(CollisionLayer layer) : layer(layer) {
            if (layer == CollisionLayer::ALL || layer == CollisionLayer::NONE)
                Application::ThrowRuntimeException("CollisionMask layer can't be ALL or NONE.", Application::INVALID_COL_LAYER);
        };

        template<typename... EnumClass>
        requires std::is_same_v<std::common_type_t<EnumClass...>, CollisionLayer>
        CollisionMask& Include(EnumClass... layers) {
            using CT = std::common_type_t<EnumClass...>;
            using UT = std::underlying_type_t<CT>;

            if (((layers == CollisionLayer::ALL) || ...))
                inclusions = CollisionLayer::ALL;
            else if (((layers == CollisionLayer::NONE) || ...))
                inclusions = CollisionLayer::NONE;
            else {
                UT merged = (static_cast<UT>(layers) | ...);
                
                if (inclusions == CollisionLayer::ALL)
                    inclusions = static_cast<CT>(merged);
                else
                    inclusions = static_cast<CT>(static_cast<UT>(inclusions) | merged);
            };

            return *this;
        };

        template<typename... EnumClass>
        requires std::is_same_v<std::common_type_t<EnumClass...>, CollisionLayer>
        CollisionMask& Clear() {
            inclusions = CollisionLayer::NONE;

            return *this;
        }

        static bool CanCollide(const CollisionMask& a, const CollisionMask& b);

        private:
        bool CanCollide(CollisionLayer layer) const;
    };
}