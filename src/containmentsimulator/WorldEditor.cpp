#include "WorldEditor.h"
#include "Button.h"

using namespace Fastboi;
using namespace CS;

WorldEditor::WorldEditor() {
    groundTileButtons.reserve(4);

    for (int i = 0; i < 4; i++) {
        Gameobject& buttonGO = Instantiate<Button::ToggleBlueprint>("TileSpritesheet", Position(400 + 100 * i, 750), Size(100, 100));
        Button& button = buttonGO.GetComponent<Button>();
        
        groundTileButtons.push_back(&button);
    }
}