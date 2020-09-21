#include "WorldEditor.h"
#include "Button.h"
#include "ScreenElement.h"
#include "TileData.h"

using namespace Fastboi;
using namespace CS;

WorldEditor::WorldEditor(GORef&& go) : go(std::move(go)) {
    groundTileButtons.reserve(4);

    constexpr Size buttonSize{100, 100};
    const ScreenElement& se = this->go().GetComponent<ScreenElement>();

    for (int i = 0; i < 6; i++) {
        TileData td = TileData::Get(static_cast<TileID>(i));

        Gameobject& buttonGO = Instantiate<Button::ToggleBlueprintC>(
            "TileSpritesheet"
            , se.anchor
            , buttonSize
            , td.cutout
        );

        buttonGO.transform->Parent(go().transform);
        buttonGO.transform->position = buttonSize * Size(i, 0);

        Button& button = buttonGO.GetComponent<Button>();
        groundTileButtons.push_back(&button);
        button.signal.connect<&WorldEditor::ButtonClick>(this);

        buttonGO.AddComponent<TileData>(td);
    }
}

void WorldEditor::ButtonClick(const ButtonEvent& e) {
    if (e.type == ButtonEvent::UNPRESS) {
        currentTileSel = TileID::NONE;
        return;
    }
    
    if (e.type == ButtonEvent::DEPRESS && currentTileSel != TileID::NONE) {
        Button& oldButton = *groundTileButtons[static_cast<std::size_t>(currentTileSel)];

        currentTileSel = TileID::NONE;
        oldButton.SetToggle(Button::Toggle::OFF); // This will cause recursion into this function but that's OK. It will go into the first if and return
    }

    currentTileSel = e.button.go().GetComponent<TileData>().id;
}

void WorldEditor::Blueprint(Gameobject& go, Position anchor) {
    go.AddComponent<Transform>(Position::zero(), Size::zero(), 0_deg);
    go.AddComponent<ScreenElement>(anchor, Size::zero());

    go.AddComponent<WorldEditor>();
}