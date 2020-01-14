#pragma once
#include "Component.h"
#include "stdint.h"
#include "memory"
#include "Gameobject.h"

struct ColorComp final {
    public:
    uint8_t r, g, b, a;

    constexpr ColorComp() : r(0), g(0), b(0), a(0) {

    };

    constexpr ColorComp(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {

    };

    void set(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    };
};

struct ColorShiftComp final {
    private:
    uint8_t count = 0;
    ColorComp* color;
    const Gameobject* gameobject;

    public:
    constexpr ColorShiftComp(const Gameobject* go) : gameobject(go), color(nullptr) { };

    void Start() {
        color = &gameobject->GetComponent<ColorComp>();
        const Transform& t = gameobject->GetComponent<Transform>();
    }

    void Update() {
        if (color != nullptr) {
            color->r += 1;
            color->g -= 4;
            color->b += 2;
        }
    }
};