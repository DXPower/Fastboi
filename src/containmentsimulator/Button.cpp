#include "Button.h"
#include "Events.h"
#include "FastboiComps.h"
#include "Rendering.h"
#include "Resources.h"

using namespace Fastboi;
using namespace CS;

Button::Button(GORef&& go, Toggle toggle)
 : go(std::move(go))
 , toggle(toggle) 
 , listener(*go().transform, *go().renderer)
 , internalListener(*go().transform, *go().renderer)
{
    internalListener.signal.connect<&Button::Click>(this);
};

void Button::Click(const TargetedClickEvent& e) {
    switch (toggle) {
        case Toggle::NONE:
            listener.signal.fire(e);

            break;
        case Toggle::OFF: {
            if (e.type == ClickEvent::UP) break;

            toggle = Toggle::ON;
            
            TargetedClickEvent down(e);
            down.type = ClickEvent::DOWN;

            listener.signal.fire(down);
            break;
        } case Toggle::ON: {
            if (e.type == ClickEvent::UP) break;

            toggle = Toggle::OFF;
            
            TargetedClickEvent up(e);
            up.type = ClickEvent::UP;

            listener.signal.fire(up);

            break;
        }
    }
}

void Button::Blueprint(Gameobject& go, const std::string& textureName, const Position& p, const Size& s) {
    go.AddComponent<Transform>(p, s, 0_deg);

    ButtonRenderer& br = go.AddComponent<ButtonRenderer>(textureName.c_str(), Rect(0, 0, 100, 100));
    Button& button = go.AddComponent<Button>(Toggle::NONE);

    button.listener.signal.connect<&ButtonRenderer::Click>(br);
}

void Button::ToggleBlueprint(Gameobject& go, const std::string& textureName, const Position& p, const Size& s) {
    go.AddComponent<Transform>(p, s, 0_deg);

    ButtonRenderer& br = go.AddComponent<ButtonRenderer>(textureName.c_str(), Rect(0, 0, 100, 100));
    Button& button = go.AddComponent<Button>(Toggle::OFF);

    button.listener.signal.connect<&ButtonRenderer::Click>(br);
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
    Rendering::RenderScreen_Texture(gameobject().transform, texture, cutout);

    if (isDepressed)
        Rendering::RenderScreen_Texture(gameobject().transform, depressedTexture, depressedCutout);
}

void ButtonRenderer::Click(const TargetedClickEvent& e) {
    if (e.type == ClickEvent::DOWN)
        isDepressed = true;
    else if (e.type == ClickEvent::UP)
        isDepressed = false;
}

Renderer& ButtonRenderer::Duplicate() const {
    return *(new ButtonRenderer(*this));
}