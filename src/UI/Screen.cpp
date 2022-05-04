#include "Screen.h"
#include "Transform.h"
#include "Gameobject.h"
#include "Renderer.h"

using namespace Fastboi;
using namespace UI;

Transform Screen::RelativizeTransform(const Transform& self) const {
    Transform parentRelative;

    if (self.HasParent()) {
        const Transform& parent = self.Parent();
        const Screen& parentScreen = parent.gameobject().renderer->screen.value();

        parentRelative = parentScreen.RelativizeTransform(parent);
    }

    Vec<float> anchor = CalculateAnchor(parentRelative);
    Vec<float> scale = CalculateScale(parentRelative);

    return Transform(anchor + self.position, self.size * scale, self.rotation);
}