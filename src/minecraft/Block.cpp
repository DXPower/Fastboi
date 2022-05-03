#include "Block.h"
#include "FastboiComps.h"
#include "World.h"

using namespace Minecraft;

struct BlockColorController {
    GORef gameobject;

    BlockColorController(GORef&& go) : gameobject(std::move(go)) { }

    void Start() {
        gameobject().GetComponent<Block>().blockUpdateSignal.connect<&BlockColorController::BlockEvent>(*this);
    }

    void BlockEvent(const BlockUpdateEvent& e) {
        if (!std::holds_alternative<BlockTypeChange>(e))
            return;

        BlockTypeChange b = std::get<BlockTypeChange>(e);

        gameobject().GetComponent<Components::ColorComp>() = Block::ColorFromType(b.next);
        gameobject().collider->SetEnabled(b.next != BlockType::Air);
    }
};

void Block::Type(BlockType newType) {
    BlockTypeChange e { .prev = type, .next = newType };
    type = newType;

    blockUpdateSignal.fire(e);
}

void Block::Inst(Gameobject& go, Vec<int> pos, BlockType type) {
    go.AddComponent<Transform>(Position(blockSize.x * pos.x, blockSize.y * pos.y), blockSize, 0_deg);
    go.AddComponent<Collider>(Collider::FIXED, CollisionLayer::WALLS);
    go.AddComponent<Components::BoxColorRenderer>(RenderData(RenderOrder::OBJECTS_UNDER));
    go.AddComponent<Components::ColorComp>(Block::ColorFromType(type));
    go.AddComponent<Block>(pos, type);
    go.AddComponent<BlockColorController>();

    if (type == BlockType::Air)
        go.collider->SetEnabled(false);
}

Components::ColorComp Block::ColorFromType(BlockType type) {
    switch (type) {
        case BlockType::Air:
            return Components::ColorComp(143, 246, 255, 255);
        case BlockType::Grass:
            return Components::ColorComp(4, 207, 28, 255);
        case BlockType::Dirt:
            return Components::ColorComp(138, 99, 74, 255);
        case BlockType::Sand:
            return Components::ColorComp(255, 212, 148, 255);
        case BlockType::Stone:
            return Components::ColorComp(148, 148, 148, 255);
        default:
            Application::ThrowRuntimeException("Invalid block type", Application::GAME_ERROR);
    }
}