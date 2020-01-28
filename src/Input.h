#pragma once

#include "stdint.h"
#include "Events.h"
#include "Transform.h"
#include <unordered_map>
#include <memory>

namespace Fastboi {
    struct Renderer;
};

namespace Fastboi {
    namespace Input {
        extern Signal<WindowResizeEvent::Signal_t_g> resizeSignal;

        //! Follows RAII. When this goes out of scope, the signal and all connections are disconnected.
        struct ClickListener {
            using EventSignature = ClickEvent::Signal_t_g;
            std::unique_ptr<Signal<EventSignature>> signal;

            ClickListener();
            ~ClickListener();
        };

        //! Follows RAII. When this goes out of scope, the signal and all connections are disconnected.
        struct TargetedClickListener {
            const Transform* transform;
            const Renderer* renderer; // Used to determine button press if there is overlap

            using EventSignature = TargetedClickEvent::Signal_t_g;
            std::unique_ptr<Signal<EventSignature>> signal;
            
            TargetedClickListener();
            ~TargetedClickListener();

            /**
             * @param transform - A click within bounds of this transform fires the signal
             * @param renderer - Used to determine which listener receives the click event in case multiple overlap
            **/
            void Init(const Transform* transform, const Renderer* renderer);

            bool operator==(const TargetedClickListener& other) const;
        };

        //! Follows RAII. When this goes out of scope, the signal and all connections are disconnected.
        struct KeyListener {
            uint32_t key;

            using EventSignature = KeyEvent::Signal_t_g;
            std::unique_ptr<Signal<EventSignature>> signal;

            KeyListener(uint32_t key);
            ~KeyListener();
        };

        void PollEvents();
        bool IsKeyDown(uint8_t key);
    };
}