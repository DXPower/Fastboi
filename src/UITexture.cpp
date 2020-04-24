#include "UITexture.h"

using namespace Fastboi;
using namespace Components;

UITexture::UITexture(GORef&& go, const char* textureName)
 : Renderer(std::forward<GORef>(go), RenderOrder::UI), texture(Resources::GetTexture(textureName)) { }

void UITexture::Render() {
    Rendering::RenderScreen_Texture(gameobject().transform, texture);
}