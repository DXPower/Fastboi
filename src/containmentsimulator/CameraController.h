#pragma once

#include "Events.h"
#include "Fastboi.h"
#include "Input.h"

namespace CS {
    using namespace Fastboi;

    class CameraController {
        private:
        GORef go;

        Vec<int> dragOrigin;
        Position camOrigin;

        bool isMouseHeld = false;

        Input::ClickListener clickListener;
        Input::MouseWheelListener wheelListener;

        public:
        CameraController(GORef&& go);

        void Start();
        void Update();

        void OnClick(const ClickEvent& event);
        void OnScroll(const MouseWheelEvent& event);

        static void Blueprint(Gameobject& go);
    };
}