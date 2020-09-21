#include "ScreenElement.h"
#include "Application.h"
#include "Gameobject.h"
#include "GORef.h"

using namespace Fastboi;

ScreenElement::ScreenElement(GORef&& go, Position anchor, Size relSize) : go(std::move(go)), anchor(anchor), relativeSize(relSize) { }

Transform ScreenElement::GetScreenTransform() const {
    Transform screenT = *go().transform;

    screenT.position += anchor * (Position) Application::GetWindowSize(); // Treat transform.position as offset from anchor
    screenT.size += relativeSize * (Position) Application::GetWindowSize(); // Treat transform.size as margin of relative screen size

    return screenT;
}