#pragma once

#include "Renderer.h"
#include "Events.h"
#include "Input.h"
#include <memory>

namespace Fastboi {
    namespace Components {
        struct ColorComp;

        struct RectUI final : public Fastboi::Renderer {
            private:
            const ColorComp* color;
            Fastboi::Input::TargetedClickListener clickListener;
            
            public:
            RectUI(Fastboi::Gameobject& go);
            RectUI(Fastboi::Gameobject& go, int z);

            void Start() override;
            void Render() override;

            void Click(const Fastboi::TargetedClickEvent& e);
        };
    };
};