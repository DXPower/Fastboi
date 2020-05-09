#include "Fastboi.h"

using namespace Fastboi;

namespace Adventure {
    struct Item;

    struct Magnet {
        GORef go;
        Item* itemSelf = nullptr;

        Magnet(GORef&& go);

        void Start();
        void Update();

        static void Inst(Gameobject& go, const Position& pos);
    };
}