#pragma once

#include "Block.h"
#include <unordered_map>

namespace Minecraft {
    struct World {
        private:
        inline static std::unordered_map<Vec<int>, Gameobject*> blocks;

        public:
        static void RegisterBlock(Gameobject& newBlock) { 
            blocks.emplace(newBlock.GetComponent<Block>().pos, &newBlock);
        }

        static void BuildWorld();

        static const decltype(blocks)& GetBlocks() { return blocks; }
        static std::optional<Gameobject*> GetBlockAtWorldPos(Position worldPos);
    };
}