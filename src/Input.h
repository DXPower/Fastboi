#pragma once

#include "Events.h"
#include <initializer_list>
#include <memory>
#include "stdint.h"
#include "Transform.h"
#include <unordered_map>
#include <vector>

namespace Fastboi {
    struct Renderer;
};

namespace Fastboi {
    namespace Input {
        extern Signal<WindowResizeEvent::Signal_t_g> resizeSignal;

        //! Follows RAII. When this goes out of scope, the signal and all connections are disconnected.
        struct ClickListener {
            using EventSignature = ClickEvent::Signal_t_g;
            mutable Signal<EventSignature> signal;

            ClickListener();
            ~ClickListener();
        };

        //! Follows RAII. When this goes out of scope, the signal and all connections are disconnected.
        struct TargetedClickListener {
            const Transform* transform;
            const Renderer* renderer; // Used to determine button press if there is overlap

            using EventSignature = TargetedClickEvent::Signal_t_g;
            mutable Signal<EventSignature> signal;
            
            private:
            TargetedClickListener(const Transform* t, const Renderer* r);

            public:
            TargetedClickListener();
            TargetedClickListener(const Transform& transform, const Renderer& renderer);
            ~TargetedClickListener();

            /**
             * @param transform - A click within bounds of this transform fires the signal
             * @param renderer - Used to determine which listener receives the click event in case multiple overlap
            **/
            void Init(const Transform& transform, const Renderer& renderer);

            // bool operator==(const TargetedClickListener& other) const;
        };

        //! Follows RAII. When this goes out of scope, the signal and all connections are disconnected.
        struct KeyListener {
            std::vector<uint32_t> keys;

            using EventSignature = KeyEvent::Signal_t_g;
            mutable Signal<EventSignature> signal;

            KeyListener(uint32_t key);
            KeyListener(std::initializer_list<uint32_t> keys);
            ~KeyListener();

            bool IsListeningToKey(uint32_t key) const;
        };

        void PollEvents();
        bool IsKeyDown(uint8_t key);
    };
}