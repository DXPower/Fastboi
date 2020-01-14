#pragma once

#include "Fastboi.h"
#include "ChangeObserver.h"

struct RepeatRenderer final : Renderer {
    private:
    ChangeObserver<Size>* observer;
    public:
    Texture* baseTexture;
    Texture* repeatTexture;
    Size tileSize;
    
    RepeatRenderer(Gameobject& gameobject, RenderData data, const char* textureName, const Size& tileSize);
    ~RepeatRenderer();

    void Start() override;
    void Render() override;

    private:
    void RecreateRepeatTexture();
    void RenderRepeatTexture(const Size& newSize);
};