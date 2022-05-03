#include "World.h"

using namespace Minecraft;

void World::BuildWorld() {
    for (int i = -10; i < 10; i++) {
        Gameobject& block = Instantiate<Block::Inst>(Vec<int>(i, 1), i < 4 ? BlockType::Grass : BlockType::Sand);
        RegisterBlock(block);
    }

    for (int i = -10; i < 10; i++) {
        Gameobject& block = Instantiate<Block::Inst>(Vec<int>(i, 2), i < 5 ? BlockType::Dirt : BlockType::Sand);
        RegisterBlock(block);
    }

    for (int i = -10; i < 10; i++) {
        Gameobject& block = Instantiate<Block::Inst>(Vec<int>(i, 3), BlockType::Dirt);
        RegisterBlock(block);
    }

    for (int i = -10; i < 10; i++) {
        Gameobject& block = Instantiate<Block::Inst>(Vec<int>(i, 4), BlockType::Stone);
        RegisterBlock(block);
    }

    for (int i = -10; i < 10; i++) {
        for (int j = 0; j < 5; j++) {
            Gameobject& block = Instantiate<Block::Inst>(Vec<int>(i, -j), BlockType::Air);
            RegisterBlock(block);
        }
    }
}

std::optional<Gameobject*> World::GetBlockAtWorldPos(Position worldPos) {
    worldPos += Block::blockSize / 2.f;

    Position rawGridPos = worldPos / Block::blockSize;
    Vec<int> gridPos(std::floor(rawGridPos.x), std::floor(rawGridPos.y));

    auto it = blocks.find(gridPos);

    if (it != blocks.end())
        return it->second;
    else
        return std::nullopt;
}
