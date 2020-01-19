#pragma once

#include "RenderOrder.h"

namespace Fastboi {
    struct Gameobject;
    void Render();

    struct Renderer {
        private:
        bool isStarted = false;
        bool isDeleted = false;

        public:
        Gameobject& gameobject;
        RenderData data;

        public:
        Renderer(Gameobject& go);
        Renderer(Gameobject& go, RenderData data);
        virtual ~Renderer();

        virtual void Start() { isStarted = true; };
        virtual void Render() = 0;
        void Destroy() { isDeleted = true; };

        RenderOrder GetOrder() const;
        void SetOrder(RenderOrder order);

        int GetZ() const;
        void SetZ(int z);

        friend void Fastboi::Render();
    };
};