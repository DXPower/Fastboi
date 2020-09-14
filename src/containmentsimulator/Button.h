#pragma once

#include "Events.h"
#include "Fastboi.h"
#include "Input.h"
#include <cstddef>
#include <stdint.h>

namespace CS {
    using namespace Fastboi;
    using namespace Input;

    class Button {
        private:
        enum class Toggle {
            NONE,
            OFF,
            ON
        };

        TargetedClickListener internalListener;
        Toggle toggle;

        public:
        GORef go;
        TargetedClickListener listener;

        Button(Fastboi::GORef&& go, Toggle toggle);

        private:
        void Click(const TargetedClickEvent& e);

        public:
        static void Blueprint(Gameobject& go, const std::string& textureName, const Position& pos, const Size& size);
        static void ToggleBlueprint(Gameobject& go, const std::string& textureName, const Position& pos, const Size& size);
    };

    class ButtonRenderer final : public Renderer {
        private:
        bool isDepressed = false;        
        const Texture& depressedTexture;
        const Rect depressedCutout = Rect(0, 640, 64, 64);

        public:
        const Texture& texture;
        Rect cutout;

        ButtonRenderer(GORef&& go, const char* textureName);
        ButtonRenderer(GORef&& go, const char* textureName, const Rect& cutout);

        void Render() override;
        void Click(const TargetedClickEvent& e);

        protected:

        ButtonRenderer(const ButtonRenderer& copy) = default;
        Renderer& Duplicate() const override;
    };
}