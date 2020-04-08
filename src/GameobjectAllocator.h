#pragma once
#include <cstdio>

namespace Fastboi {
    struct Gameobject;

    class GameobjectAllocator {
        struct Chunk;
        struct Block;

        std::size_t chunksPerBlock;
        Chunk* allocChunk = nullptr;

        Block* AllocateBlock();

        public:
        GameobjectAllocator(std::size_t chunksPerBlock) : chunksPerBlock(chunksPerBlock) { };

        void* Allocate();
        void Deallocate(void* del);
    };
}