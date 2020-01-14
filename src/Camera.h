#pragma once

#include "Gameobject.h"
#include "Vec.h"
#include <memory>
#include <type_traits>

namespace Fastboi {
    class Camera;
    extern Camera camera;

    // This determines whether the Camera should destroy its holding Transform when it itself is destroyed
    enum class CameraTarget {
        OWNING,
        WATCHING
    };

    void SetCamera(Camera&& camera);

    class Camera final {
        protected:
        const Transform* target;
        CameraTarget type;
        mutable Vec<int> window;

        public:
        Camera();
        Camera(const Transform& target, CameraTarget type);
        ~Camera();
        
        void SetTarget(const Transform& target, CameraTarget type);
        const Transform& GetTarget() const;

        Position WorldToScreenPos(const Position& worldPos) const;
        Position ScreenToWorldPos(const Position& screenPos) const;
        bool IsPointVisible(const Position& worldPos) const;
    };
};