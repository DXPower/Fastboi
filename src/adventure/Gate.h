#pragma once

#include "Fastboi.h"

using namespace Fastboi;
using namespace Fastboi::Components;

namespace Fastboi::Components {
    struct SpriteRenderer;
}

namespace Adventure {
    enum class KeyColor;
    struct Key;
    struct Room;

    struct Gate {
        using Reqs = RequiredComponents<SpriteRenderer, Collider>;

        private:
        enum class GateState {
            OPEN, CLOSED
        };

        GateState curState = GateState::CLOSED;
        float timeElapsed = 10.0f;

        Gameobject* holder = nullptr;
        Gameobject* key = nullptr;

        SpriteRenderer* renderer = nullptr;

        constexpr static unsigned char MIN_PROGRESS = 0;
        constexpr static unsigned char MAX_PROGRESS = 6;

        const Room& inside;
        bool hasEntered = false;

        public:
        const KeyColor color;
        Fastboi::GORef go;

        Gate(GORef&& go, KeyColor color, const Room& inside);

        void Start();
        void Update();

        void TryOpen(const Key& key);
        void TryEnterGate(Gameobject& pl);

        void EnterTriggerHit(const CollisionEvent& e);
        void Collision(const CollisionEvent& e);

        static void Inst(Gameobject& go, const Position& pos, KeyColor color, const Room& inside);

        private:
        void SetProgress(unsigned char progress);
    };
}