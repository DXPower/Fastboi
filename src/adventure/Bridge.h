#include "Fastboi.h"

using namespace Fastboi;

namespace Adventure {
    struct Bridge {
        GORef go;

        Bridge(GORef&& go);

        void Start();
        void Update();

        void EnterBridge(const CollisionEvent& e);

        static void Inst(Gameobject& go, const Position& pos);
    };
}