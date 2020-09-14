#include "UITexture.h"

using namespace Fastboi;
using namespace Components;

UITexture::UITexture(GORef&& go, const char* textureName)
 : Renderer(std::move(go), RenderOrder::UI)
 , texture(Resources::GetTexture(textureName))
 , cutout(0, 0, texture.GetSize().x, texture.GetSize().y) 
 { }

UITexture::UITexture(GORef&& go, const char* textureName, const Rect& cutout)
 : Renderer(std::move(go), RenderOrder::UI)
 , texture(Resources::GetTexture(textureName))
 , cutout(cutout) 
 { }

void UITexture::Render() {
    Rendering::RenderScreen_Texture(gameobject().transform, texture, cutout);
}

Renderer& UITexture::Duplicate() const {
    return *(new UITexture(*this));
}