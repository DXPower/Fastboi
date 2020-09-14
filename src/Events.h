#pragma once

#include <memory>
#include "nano_ss/nano_signal_slot.hpp"
#include "Transform.h"
#include <Vec.h>

namespace Fastboi {
    struct Collider;
    struct Gameobject;
    
    using Nano::Signal;

    struct KeyEvent final {
        using Signal_t_g = void(const KeyEvent&);

        enum KeyEventType {
            UP = 1,
            DOWN = 2
        };

        uint32_t key;
        KeyEventType type;

        constexpr KeyEvent(uint32_t key, KeyEventType type) : key(key), type(type) { };

        bool operator==(const KeyEvent& rhs) const {
            return key == rhs.key && type == rhs.type;
        };

        struct Hash {
            size_t operator()(const KeyEvent& e) const {
                return e.key << 4 | (uint32_t) e.type;
            }
        };
    };

    struct CollisionEvent final {
        using Signal_t_g = void(const CollisionEvent&);

        enum CollisionType {
            BEGIN,
            END
        };

        Collider& collider;
        CollisionType type;
        Vecf penetration;

        CollisionEvent(Collider& collider, CollisionType type, const Vecf& pen) : collider(collider), type(type), penetration(pen) { };
    };

    struct ClickEvent final {
        using Signal_t_g = void(const ClickEvent&);

        enum ClickType {
            UP,
            DOWN
        };

        ClickType type;
        int button;
        int clicks;
        Vec<int> pos;

        ClickEvent(ClickType type, int button, int clicks, const Vec<int>& pos)
        : type(type), button(button), clicks(clicks), pos(pos) { };
    };
    
    struct TargetedClickEvent final {
        using Signal_t_g = void(const TargetedClickEvent&);

        Transform& transform;
        ClickEvent::ClickType type;
        int button;
        int clicks;
        Vec<int> pos;

        TargetedClickEvent(Transform& t, ClickEvent::ClickType type, int button, int clicks, const Vec<int>& pos)
        : transform(t), type(type), button(button), clicks(clicks), pos(pos) { };
    };

    struct MouseWheelEvent final {
        using Signal_t_g = void(const MouseWheelEvent&);


        const Vec<int> scroll;
    };

    // Intended to help objects get rid of dangling pointers
    struct GameobjectDeleteEvent final {
        using Signal_t_g = void(const GameobjectDeleteEvent&);

        const Gameobject& go;
    };

    struct WindowResizeEvent final {
        using Signal_t_g = void (const WindowResizeEvent&);

        const Vec<int> size;
    };
};
