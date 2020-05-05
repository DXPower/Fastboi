#pragma once

#include <array>
#include "Fastboi.h"
#include "Timer.h"

using namespace Fastboi;
using namespace Fastboi::Components;

namespace Fastboi::Components {
    struct SpriteRenderer;
}

namespace Adventure {
    struct DragonStats {
        int health = 0;
        float speed = 0;

        DragonStats(int health, float speed) : health(health), speed(speed) { }
    };
    
    struct Yorgle;
    struct Dragon {
        using Reqs = RequiredComponents<Collider, Rigidbody, SpriteRenderer>;

        private:
        struct Jaws {
            GORef go;

            Jaws(GORef&& go);
        };

        DragonStats stats;

        Gameobject& player;
        Rigidbody* rb;

        Gameobject* jaws;
        Gameobject* belly;
        std::array<Gameobject*, 2> mouthPieces;


        Timer biteTimer;
        bool doChase = true;
        bool isBiting = false;
        bool isDead = false;

        public:
        GORef go;

        Dragon(GORef&& go, Gameobject& player, const DragonStats& stats);

        void Start();
        void Update();

        void CloseJaws();
        void Bite();
        void SwordHit();
        Position GetBiteLocation() const;

        inline void SetChase(bool f) { doChase = f; };
        inline bool IsDead() const { return stats.health <= 0; };

        void Collision(const CollisionEvent& e);

        private:
        static void InstDelegate(Gameobject& go, Gameobject& player, const Position& pos, const DragonStats& stats);

        friend struct Yorgle;
    };

    struct Yorgle : DragonStats {
        static void Inst(Gameobject& go, Gameobject& player, const Position& pos);

        private:
        Yorgle();
    };
}