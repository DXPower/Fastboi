#include "Camera.h"
#include "Application.h"

using namespace Fastboi;

namespace Fastboi {
    const Transform nullTransform;
    const Camera nullCamera(nullTransform );
    
    const Camera* mainCamera = &nullCamera;
}

void Fastboi::SetCamera(const Camera& cam) {
    mainCamera = &cam;
}

const Camera& Fastboi::GetCamera() {
    return *mainCamera;
}

static void CameraDestroyed(const Camera& cam) {
    if (mainCamera == &cam && &cam != &nullCamera)
        Application::ThrowRuntimeException("Main camera destroyed", Application::NO_CAMERA);
}

Camera::Camera(GORef&& go) : gameobject(std::move(go)), target(nullptr) { };

Camera::Camera(GORef&& go, float zoom) : gameobject(std::move(go)), zoom(zoom) { };
Camera::Camera(const Transform& target, float zoom) : target(&target), zoom(zoom) { };

Camera::~Camera() {
    CameraDestroyed(*this);
}

void Camera::Start() {
    if (target == nullptr)
        this->target = &gameobject().GetComponent<Transform>();
}

Position Camera::WorldToScreenPos(const Position& worldPos) const {
    return (worldPos - target->position) * zoom + (Vecf) (Application::GetWindowSize() / 2);
};

Position Camera::ScreenToWorldPos(const Position& screenPos) const {
    return  (screenPos - (Vecf) (Application::GetWindowSize() / 2)) / zoom + target->position; 
};

bool Camera::IsPointVisible(const Position& worldPos) const {
    Position screenPos = WorldToScreenPos(worldPos);
    return screenPos.x >= 0 && screenPos.x < Application::GetWindowSize().x
        && screenPos.y >= 0 && screenPos.y < Application::GetWindowSize().y;
}

BoundingBox Camera::GetVisibleBounds() const {
    BoundingBox bounds;

    bounds.lowerBounds = ScreenToWorldPos(Position(0, 0));
    bounds.upperBounds = ScreenToWorldPos((Position) Application::GetWindowSize());

    return bounds;
}
