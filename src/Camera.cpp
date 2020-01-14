#include "Camera.h"
#include "Application.h"

using namespace Fastboi;

Fastboi::Camera Fastboi::camera;

void Fastboi::SetCamera(Camera&& cam) {
    Fastboi::camera = std::move(cam);
}

Camera::Camera() : target(nullptr), type(CameraTarget::WATCHING) { };

Camera::Camera(const Transform& target, CameraTarget type) : target(&target), type(type) {
    Application::GetWindowSize(&window.x, &window.y);    
};

Camera::~Camera() {
    if (type == CameraTarget::OWNING)
        delete target;
}

void Camera::SetTarget(const Transform& target, CameraTarget type) { 
    printf("Setting target as %s\n", type == CameraTarget::OWNING ? "OWNING" : "WATCHING");
    if (this->type == CameraTarget::OWNING)
        delete this->target;
    
    this->target = &target;
    this->type = type;
}

const Transform& Camera::GetTarget() const { 
    return *target;
};

Position Camera::WorldToScreenPos(const Position& worldPos) const {
    return worldPos - target->position + (Vecf) (window / 2);
};

Position Camera::ScreenToWorldPos(const Position& screenPos) const {
    return  screenPos + target->position - (Vecf) (window / 2);
};

bool Camera::IsPointVisible(const Position& worldPos) const {
    Position screenPos = WorldToScreenPos(worldPos);
    return screenPos.x >= 0 && screenPos.x < window.x
        && screenPos.y >= 0 && screenPos.y < window.y;
}