#include "Camera.h"
#include "Application.h"

using namespace Fastboi;

Fastboi::Camera Fastboi::camera;

void Fastboi::SetCamera(const Camera& cam) {
    printf("Setting camera...\n");
    Fastboi::camera.~Camera(); // Destroy the camera in-place, without memory deallocation
    std::memcpy(&Fastboi::camera, &cam, sizeof(Camera)); // Do byte-by-byte copy because we can't use operator=, due to window being ref
    //TODO: Rework camera creation
}

Camera::Camera() : target(nullptr), type(CameraTarget::WATCHING) { };

Camera::Camera(const Transform& target, CameraTarget type) : target(&target), type(type), zoom(1.f) {
    // Application::GetWindowSize(&window.x, &window.y);    
};

Camera::Camera(const Transform& target, CameraTarget type, float zoom) : target(&target), type(type), zoom(zoom) {
    // Application::GetWindowSize(&window.x, &window.y);    
};

Camera::~Camera() {
    printf("Deleting camera...\n");

    if (type == CameraTarget::OWNING) {
        printf("Deleting target...\n");
        delete target;
    }
}

void Camera::SetTarget(const Transform& target, CameraTarget type) { 
    if (this->type == CameraTarget::OWNING)
        delete this->target;
    
    this->target = &target;
    this->type = type;
}

const Transform& Camera::GetTarget() const { 
    return *target;
};

Position Camera::WorldToScreenPos(const Position& worldPos) const {
    return (worldPos - target->position) * zoom + (Vecf) (window / 2);
};

Position Camera::ScreenToWorldPos(const Position& screenPos) const {
    return  (screenPos - (Vecf) (window / 2)) / zoom + target->position; 
};

bool Camera::IsPointVisible(const Position& worldPos) const {
    Position screenPos = WorldToScreenPos(worldPos);
    return screenPos.x >= 0 && screenPos.x < window.x
        && screenPos.y >= 0 && screenPos.y < window.y;
}

BoundingBox Camera::GetVisibleBounds() const {
    BoundingBox bounds;

    bounds.lowerBounds = ScreenToWorldPos(Position(0, 0));
    bounds.upperBounds = ScreenToWorldPos((Position) window);

    return bounds;
}
