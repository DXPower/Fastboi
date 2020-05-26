#pragma once

#include "Gameobject.h"
#include "Vec.h"
#include "Application.h"
#include <memory>
#include <type_traits>

namespace Fastboi {
    class Camera;
    extern Camera camera;

    void SetCamera(const Camera& camera);

    struct Camera final {
        // This determines whether the Camera should destroy its holding Transform when it itself is destroyed
        enum CameraTarget {
            OWNING,
            WATCHING
        };
        
        private:
        const Transform* target;
        const Vec<int>& window = Application::GetWindowSize();
        CameraTarget type;

        public:
        float zoom = 1.f;

        Camera();
        Camera(const Transform& target, CameraTarget type);
        Camera(const Transform& target, CameraTarget type, float zoom);
        ~Camera();
        
        void SetTarget(const Transform& target, CameraTarget type);
        const Transform& GetTarget() const;

        Position WorldToScreenPos(const Position& worldPos) const;
        Position ScreenToWorldPos(const Position& screenPos) const;
        bool IsPointVisible(const Position& worldPos) const;

        BoundingBox GetVisibleBounds() const;
    };
};