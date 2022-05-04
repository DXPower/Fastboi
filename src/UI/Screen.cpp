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

    Vec<float> scale = CalculateScale(self.size, parentRelative);
    Size newSize = self.size * scale;
    
    Vec<float> anchor = CalculateAnchor(newSize, parentRelative);

    return Transform(anchor + self.position, newSize, self.rotation);
}