#include "Button.h"
#include "Events.h"
#include "FastboiComps.h"
#include "Rendering.h"
#include "Resources.h"

using namespace Fastboi;
using namespace CS;

ButtonEvent::ButtonEvent(Button& button) : button(button) { }
ButtonEvent::ButtonEvent(Button& button, ButtonEventType type, bool isToggle) : button(button), type(type), isToggle(isToggle) { }

Button::Button(GORef&& go, Toggle toggle)
 : go(std::move(go))
 , toggle(toggle) 
 , internalListener(*go().transform, *go().renderer)
{
    internalListener.signal.connect<&Button::Click>(this);
};

void Button::Click(const TargetedClickEvent& e) {
    ButtonEvent bevent = ButtonEvent(*this);

    if (toggle == Toggle::NONE) {
        bevent.type = e.type == ClickEvent::DOWN ? ButtonEvent::DEPRESS : ButtonEvent::UNPRESS;
        bevent.isToggle = false;
    } else if (e.type == ClickEvent::DOWN) {
        bevent.isToggle = true;
        
        if (toggle == Toggle::OFF) {
            toggle = Toggle::ON;
            bevent.type = ButtonEvent::DEPRESS;
        } else {
            toggle = Toggle::OFF;
            bevent.type = ButtonEvent::UNPRESS;
        }
    }

    signal.fire(bevent);
}

void Button::SetToggle(Toggle t) {
    if (toggle == t) return;

    ButtonEvent bevent = ButtonEvent(*this);
    bevent.isToggle = true;

    if (toggle == Toggle::OFF && t == Toggle::ON) {
        bevent.type = ButtonEvent::DEPRESS;
    } else if (toggle == Toggle::ON && t == Toggle::OFF) {
        bevent.type = ButtonEvent::UNPRESS;
    }

    signal.fire(bevent);
}

void Button::Blueprint(Gameobject& go, const std::string& textureName, const Position& anchor, const Size& s) {
    go.AddComponent<Transform>(Position::zero(), s, 0_deg);
    go.AddComponent<ScreenElement>(anchor, Size::zero());

    ButtonRenderer& br = go.AddComponent<ButtonRenderer>(textureName.c_str(), Rect(0, 0, 100, 100));
    Button& button = go.AddComponent<Button>(Toggle::NONE);

    button.signal.connect<&ButtonRenderer::Click>(br);
}

void Button::BlueprintC(Gameobject& go, const std::string& textureName, const Position& anchor, const Size& s, const Rect& cutout) {
    Button::Blueprint(go, textureName, anchor, s);

    go.GetComponent<ButtonRenderer>().cutout = cutout;
}


void Button::ToggleBlueprint(Gameobject& go, const std::string& textureName, const Position& anchor, const Size& s) {
    go.AddComponent<Transform>(Position::zero(), s, 0_deg);
    go.AddComponent<ScreenElement>(anchor, Size::zero());

    ButtonRenderer& br = go.AddComponent<ButtonRenderer>(textureName.c_str(), Rect(0, 0, 100, 100));
    Button& button = go.AddComponent<Button>(Toggle::OFF);

    button.signal.connect<&ButtonRenderer::Click>(br);
}

void Button::ToggleBlueprintC(Gameobject& go, const std::string& textureName, const Position& anchor, const Size& s, const Rect& cutout) {
    Button::ToggleBlueprint(go, textureName, anchor, s);

    go.GetComponent<ButtonRenderer>().cutout = cutout;
}

ButtonRenderer::ButtonRenderer(GORef&& go, const char* textureName)
 : ButtonRenderer(std::move(go), textureName, Rect(0, 0, texture.GetSize().x, texture.GetSize().y)) { }

ButtonRenderer::ButtonRenderer(GORef&& go, const char* textureName, const Rect& cutout)
 : Renderer(std::move(go), RenderOrder::UI)
 , texture(Resources::GetTexture(textureName))
 , cutout(cutout)
 , depressedTexture(Resources::GetTexture("TileSpritesheet"))
 { }

void ButtonRenderer::Render() {
    Transform screenT = gameobject().GetComponent<ScreenElement>().GetScreenTransform();
    Rendering::RenderScreen_Texture(screenT, texture, cutout);

    if (isDepressed)
        Rendering::RenderScreen_Texture(screenT, depressedTexture, depressedCutout);
}

void ButtonRenderer::Click(const ButtonEvent& e) {
    if (e.type == ButtonEvent::DEPRESS)
        isDepressed = true;
    else if (e.type == ButtonEvent::UNPRESS)
        isDepressed = false;
}

Renderer& ButtonRenderer::Duplicate() const {
    return *(new ButtonRenderer(*this));
}