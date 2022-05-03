#pragma once

#include "Fastboi.h"
#include <variant>

using namespace Fastboi;

namespace Minecraft {
    enum class BlockType {
          Air
        , Grass
        , Dirt
        , Sand
        , Stone
    };

    class BlockTypeChange;
    using BlockUpdateEvent = std::variant<BlockTypeChange>;

    struct Block {
        static constexpr Size blockSize = Size(100.f, 100.f);

        using BlockUpdateSignal_g = void(const BlockUpdateEvent&);
        Fastboi::Signal<BlockUpdateSignal_g> blockUpdateSignal;

        Vec<int> pos;

        private:
        BlockType type;

        public:
        Block(Vec<int> pos, BlockType type) : pos(pos), type(type) { }

        void Type(BlockType type);
        BlockType Type() const { return type; }

        
        static Components::ColorComp ColorFromType(BlockType type);
        static void Inst(Gameobject& go, Vec<int> pos, BlockType type);
    };

    struct BlockTypeChange {
        BlockType prev, next;
    };

}

namespace std {
    template<typename T>
    struct hash<Vec<T>> {
        std::size_t operator()(const Vec<T>& v) const {
            return (std::hash<int>()(v.x) * 0x3D30F19CD101) ^ (std::hash<int>()(v.y) * 0x7FFFFFFF);
        }
    };
}