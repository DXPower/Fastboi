#pragma once

#include "GORef.h"
#include "Input.h"
#include "Gameobject.h"

namespace Fastboi {
    namespace Components {
        struct ClickTarget {
            private:
            Input::TargetedClickListener listener;

            public:
            void Start(Gameobject& go) {
                listener.Init(*go.transform, *go.renderer);
            }

            auto& Signal() {
                return listener.signal;
            }
        };
    }
}