#pragma once

#include "Gameobject.h"
#include "Vec.h"
#include "Application.h"
#include <memory>
#include <type_traits>

namespace Fastboi {
    struct Camera;

    void SetCamera(const Camera& camera);
    const Camera& GetCamera();
    
    struct Camera final {
        GORef gameobject;
        const Transform* target = nullptr;
        float zoom = 1.f;

        Camera(GORef&& go);
        Camera(GORef&& go, float zoom = 1.f);
        Camera(const Transform& target, float zoom = 1.f);
        ~Camera();

        void Start();

        Position WorldToScreenPos(const Position& worldPos) const;
        Position ScreenToWorldPos(const Position& screenPos) const;
        bool IsPointVisible(const Position& worldPos) const;

        BoundingBox GetVisibleBounds() const;
    };
};