#pragma once

#include "Gameobject.h"
#include "Vec.h"
#include "Application.h"
#include <memory>
#include <type_traits>

namespace Fastboi {
    class Camera;

    void SetCamera(const Camera& camera);
    const Camera& GetCamera();
    
    struct Camera final {
        const Transform* target;
        float zoom = 1.f;

        Camera();
        Camera(float zoom = 1.f);
        Camera(const Transform& target, float zoom = 1.f);
        ~Camera();

        void Start(const Gameobject& go);

        Position WorldToScreenPos(const Position& worldPos) const;
        Position ScreenToWorldPos(const Position& screenPos) const;
        bool IsPointVisible(const Position& worldPos) const;

        BoundingBox GetVisibleBounds() const;
    };
};