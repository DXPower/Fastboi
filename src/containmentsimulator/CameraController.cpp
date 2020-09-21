#include "CameraController.h"
#include "Camera.h"
#include "FastboiCore.h"
#include "Input.h"

using namespace CS;
using namespace Fastboi;
using namespace Input;

CameraController::CameraController(GORef&& go) : go(std::move(go)), clickListener(ClickListener()) {
    clickListener.signal.connect<&CameraController::OnClick>(this);
    wheelListener.signal.connect<&CameraController::OnScroll>(this);
}

void CameraController::Start() {
    Fastboi::camera.SetTarget(*go().transform, Camera::WATCHING);
    Fastboi::camera.zoom = 1.f;
}

void CameraController::Update() {
    if (isMouseHeld) {
        const Vec<int>& movedPos = GetMousePosition();
        Vecf delta = static_cast<Vecf>(dragOrigin - movedPos) * (2 - Fastboi::camera.zoom);

        go().transform->position = camOrigin + delta;
    }
}

void CameraController::OnClick(const ClickEvent& e) {
    if (e.button == 3) {
        if (e.type == ClickEvent::DOWN) {
            dragOrigin = GetMousePosition();
            isMouseHeld = true;

            Input::BlockTargetedMouseUp(); // Don't let buttons absorb the mouse up
        } else if (e.type == ClickEvent::UP) {
            isMouseHeld = false;
            camOrigin = go().transform->position;
        }
    }
}

void CameraController::OnScroll(const MouseWheelEvent& e) {
    printf("Scroll: %i %i\n", e.scroll.x, e.scroll.y);

    Fastboi::camera.zoom += (float) e.scroll.y / 10.f;
}

void CameraController::Blueprint(Gameobject& go) {
    go.name = "Camera";

    go.AddComponent<Transform>(Position::zero());
    go.AddComponent<CameraController>();
}