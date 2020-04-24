#pragma once

#include "RenderOrder.h"
#include "GORef.h"

namespace Fastboi {
    struct Gameobject;

    void Render();

    struct Renderer {
        private:
        bool isStarted = false;
        bool isDeleted = false;

        public:
        GORef gameobject;
        RenderData data;

        public:
        Renderer(GORef&& go);
        Renderer(GORef&& go, RenderData data);
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