#pragma once
#include "GORef.h"
#include "Transform.h"
#include "stdint.h"

namespace Fastboi {
    namespace Components {
        struct ColorComp final {
            public:
            uint8_t r, g, b, a;

            constexpr ColorComp() : ColorComp(0, 0, 0, 0) { };
            constexpr ColorComp(const GORef&&) : ColorComp() { };
            constexpr ColorComp(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) { };
            constexpr ColorComp(const GORef&&, uint8_t r, uint8_t g, uint8_t b, uint8_t a) : ColorComp(r, g, b, a) { };

            void set(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
                this->r = r;
                this->g = g;
                this->b = b;
                this->a = a;
            };
        };

        struct ColorShiftComp final {
            uint8_t count = 0;
            ColorComp* color;

            GORef gameobject;
            ColorShiftComp(GORef&& go) : gameobject(std::move(go)), color(nullptr) { };

            void Start() {
                printf("Got to colorshiftcomp start\n");
                printf("GO address: %p\n", gameobject.go);
                printf("Color shift comp, GO name: %s\n", gameobject().name.c_str());
                color = &gameobject().GetComponent<ColorComp>();
                const Transform& t = gameobject().GetComponent<Transform>();
            }

            void Update() {
                if (color != nullptr) {
                    color->r += 1;
                    color->g -= 4;
                    color->b += 2;
                }
            }
        };
    };
};