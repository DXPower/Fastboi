#pragma once

#include "Events.h"
#include "Fastboi.h"
#include "Input.h"
#include <cstddef>
#include <stdint.h>

namespace CS {
    using namespace Fastboi;
    using namespace Input;

    class Button;

    class ButtonEvent {
        public:
        enum ButtonEventType {
            DEPRESS,
            UNPRESS
        };

        using Signal_t_g = void(const ButtonEvent&);

        Button& button;
        ButtonEventType type;
        bool isToggle;

        ButtonEvent(Button& button);
        ButtonEvent(Button& button, ButtonEventType type, bool isToggle);
    };

    class Button {
        public:
        enum class Toggle {
            NONE,
            OFF,
            ON
        };

        private:
        TargetedClickListener internalListener;
        Toggle toggle;

        public:
        GORef go;
        Signal<ButtonEvent::Signal_t_g> signal;

        Button(Fastboi::GORef&& go, Toggle toggle);

        void SetToggle(Toggle t);

        private:
        void Click(const TargetedClickEvent& e);

        public:
        static void Blueprint(Gameobject& go, const std::string& textureName, const Position& anchor, const Size& size);
        static void BlueprintC(Gameobject& go, const std::string& textureName, const Position& anchor, const Size& size, const Rect& cutout);
        static void ToggleBlueprint(Gameobject& go, const std::string& textureName, const Position& anchor, const Size& size);
        static void ToggleBlueprintC(Gameobject& go, const std::string& textureName, const Position& anchor, const Size& size, const Rect& cutout);
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
        void Click(const ButtonEvent& e);

        protected:
        ButtonRenderer(const ButtonRenderer& copy) = default;
        Renderer& Duplicate() const override;
    };
}