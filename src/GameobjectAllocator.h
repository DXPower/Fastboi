#pragma once
#include <cstdio>
#include <vector>

namespace Fastboi {
    struct Gameobject;

    class GameobjectAllocator {
        struct Chunk;
        struct Block;

        std::size_t chunksPerBlock;
        Chunk* allocChunk = nullptr;
        Chunk* unstartedHead = nullptr;
        Chunk* unstartedTail = nullptr;
        Block* blockHead = nullptr;
        Block* blockTail = nullptr;
        

        public:
        GameobjectAllocator();

        void* Allocate();
        void Deallocate(void* del);
        void StartAll();


        private:
        void MarkUnstarted(Chunk& chunk);
        Block* AllocateBlock();

        public:
        class iterator {
            Block* block;
            std::size_t cn = 0;

            public:
            iterator(Block* block) : block(block) { };

            iterator operator++(int _);
            Gameobject& operator*() const;

            bool operator==(const iterator& other) const { return block == other.block; };
            bool operator!=(const iterator& other) const { return !(*this == other); };
        };
        
        iterator GO_Begin() { return iterator(blockHead); };
        iterator GO_End() { return iterator(nullptr); };
    };
}