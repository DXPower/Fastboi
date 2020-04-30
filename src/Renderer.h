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

        public:
        virtual void Start() { isStarted = true; };
        virtual void Render() = 0;

        RenderOrder GetOrder() const;
        void SetOrder(RenderOrder order);

        int GetZ() const;
        void SetZ(int z);

        void Destroy() { isDeleted = true; };

        protected:
        Renderer(const Renderer& copy);
        virtual Renderer& Duplicate() const = 0;

        friend void Fastboi::Render();
        friend class Gameobject;
    };
};