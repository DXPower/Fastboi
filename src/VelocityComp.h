#pragma once

#include <Vec.h>

namespace Fastboi {
    struct VelocityComp {
        private:
        bool isEnabled = true;

        public:
        Velocity velocity;

        VelocityComp(const Gameobject&, const Velocity& v) : velocity(v) { };
        VelocityComp(const Gameobject& go) : VelocityComp(go, Velocity::zero()) { };

        inline void SetEnabled(bool f) { isEnabled = f; };
        inline void Enable() { SetEnabled(true); };
        inline void Disable() { SetEnabled(false); };
        inline bool IsEnabled() const { return isEnabled; };
    };
};