#include "Inventory.h"
#include "BoxColorRenderer.h"
#include "FastboiComps.h"
#include "FastboiCore.h"
#include "RenderOrder.h"

using namespace Fastboi;
using namespace Components;
using namespace Minecraft;

void InventorySquare::Inst(Gameobject& go, std::size_t index) {
    go.AddComponent<Transform>(Position::zero(), Size(1, 1), 0_deg);
    go.AddComponent<BoxColorRenderer>(RenderOrder::UI);
    go.AddComponent<InventorySquare>().index = index;
    

    auto& spriteHolder = Instantiate<Gameobject>();

    spriteHolder.AddComponent<Transform>(Position::zero(), Size(1, 1), 0_deg);
    
    // auto& r = spriteHolder.AddComponent<SpriteRenderer>(Args &&args...)
}