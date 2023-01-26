#pragma once

#include <array>
#include "Events.h"
#include "Fastboi.h"
#include "FastboiComps.h"
#include "Item.h"

using namespace Fastboi;

namespace Minecraft {
    struct Inventory {
        std::array<std::unique_ptr<Stack>, 5> stacks;

        int selection = 0;

        void ClickEvent(const TargetedClickEvent& e);
        void KeyEvent(const KeyEvent& e);
    };

    struct InventorySquare {
        std::size_t index;

        static void Inst(Gameobject& go, std::size_t index);
    };
}