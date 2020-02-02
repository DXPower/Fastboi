#include "UITexture.h"

using namespace Fastboi;
using namespace Components;

UITexture::UITexture(Gameobject& go, const char* textureName)
 : Renderer(go, RenderOrder::UI), texture(Resources::GetTexture(textureName)) { }

void UITexture::Render() {
    Rendering::RenderScreen_Texture(gameobject.transform, texture);
}