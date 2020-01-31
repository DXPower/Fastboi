#pragma once

#include "Gameobject.h"

namespace Fastboi {
    template <typename... Types>
    struct RequiredComponents final {
        public:
        RequiredComponents() = delete; // Not constructible

        static bool HasRequiredComponents(const Gameobject& go) {
            if constexpr (sizeof...(Types) == 0)
                return true;
            
            return (go.HasComponent<Types>() && ...); // Fold expression over each required component
        };
    };

};

