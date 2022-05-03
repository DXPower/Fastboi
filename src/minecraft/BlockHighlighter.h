#pragma once

#include "Block.h"
#include "FastboiComps.h"

namespace Minecraft {
    struct BlockHighlighter {
        GORef gameobject;
        Gameobject* curBlock = nullptr;

        Input::TargetedClickListener clickListener;

        BlockHighlighter(GORef&& go) : gameobject(std::move(go)) {
            clickListener.signal.connect<&BlockHighlighter::Click>(*this);
        }

        void Start() {
            clickListener.Init(*gameobject().transform, *gameobject().renderer);
        }

        void Update() {
            const Vec<int>& mousePos = Input::GetMousePosition();
            Position worldPos = GetCamera().ScreenToWorldPos(mousePos);

            std::optional<Gameobject*> hoveredBlock = World::GetBlockAtWorldPos(worldPos);

            if (!hoveredBlock.has_value()) {
                curBlock = nullptr;
                gameobject().renderer->SetEnabled(false);
                return;
            }

            curBlock = *hoveredBlock;

            gameobject().transform->position = hoveredBlock.value()->transform->position;
            gameobject().renderer->SetEnabled(true);
        }

        void Click(const TargetedClickEvent& e) {
            if (e.type == ClickEvent::ClickType::DOWN && curBlock != nullptr) {
                curBlock->GetComponent<Block>().Type(BlockType::Sand);
            }
        }

        static void Inst(Gameobject& go) {
            go.name = "Highlighter";

            go.AddComponent<Transform>(Position::zero(), Block::blockSize, 0_deg);
            
            auto& renderer = go.AddComponent<Components::BoxColorRenderer>(RenderData(RenderOrder::OBJECTS_UNDER, 10));
            go.AddComponent<Components::ColorComp>(220, 220, 220, 80);

            renderer.SetEnabled(false);

            go.AddComponent<BlockHighlighter>();
        }
    };
}