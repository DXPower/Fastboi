#include "Button.h"
#include "Gameobject.h"
#include "BoxColorRenderer.h"
#include "SpriteRenderer.h"
#include "CommonLayouts.h"
#include "ClickTarget.h"

using namespace Fastboi;
using namespace Components;
using namespace Fastboi::UI;

void Components::ColorButton(Gameobject& go, const ColorComp& color) {
    go.AddComponent<Transform>(Position(0, 0), Size(1, 1), 0_deg);

    auto& renderer = go.AddComponent<BoxColorRenderer>(RenderData(RenderOrder::UI));
    go.AddComponent<ColorComp>(color);

    renderer.screen = Screen(
        Layouts::Centered::Screen,
        Scale{ScreenConst(100.f), ScreenConst(100.f)}
    );


    go.AddComponent<ClickTarget>();
}

void Components::TextureButton(Gameobject& go, const Texture& texture) {
    go.AddComponent<Transform>(Position(0, 0), Size(1, 1), 0_deg);

    const auto [sx, sy] = texture.GetSize();
    auto& renderer = go.AddComponent<SpriteRenderer>(RenderData(RenderOrder::UI), texture, Rect(0, 0, sx, sy));

    renderer.screen = Screen(
        Layouts::Centered::Screen,
        Scale{ScreenConst(100.f), ScreenConst(100.f)}
    );

    go.AddComponent<ClickTarget>();
}