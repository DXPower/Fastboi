#include "GameobjectAllocator.h"
#include "Gameobject.h"
#include <variant>
#include <vector>

using namespace Fastboi;

struct GameobjectAllocator::Chunk {
    Gameobject allocation; // Must keep this as first member
    Chunk* nextStart; // Used to keep track of all gameobjects that need to be started

    static void Init(Chunk& chunk, Chunk* alloc) {
        printf("Initting Chunk location...: %p\n", &chunk);
        *reinterpret_cast<Chunk**>(&chunk.allocation) = alloc; // First memory in the chunk is now the allocation pointer to the next chunk
        chunk.nextStart = nullptr;

        printf("Chunk location: %p\n", &chunk);
    }
};

// Used only for defining the locations of variables inside the malloced creation in AllocateBlock
struct GameobjectAllocator::Block {
    Block* nextBlock;
    size_t numChunks;
    Chunk chunks; // Chunks begin immediately here in memory.
};

void* GameobjectAllocator::Allocate() {
    if (allocChunk == nullptr) {
        allocChunk = &AllocateBlock()->chunks;
    }

    void* ret = static_cast<void*>(allocChunk);
    printf("Return chunk: %p\n", ret);
    allocChunk = *reinterpret_cast<Chunk**>(ret);

    return ret;
}

void GameobjectAllocator::Deallocate(void* go) {
    printf("Deallocating chunk: %p\n", go);
    // Chunk* chunk = reinterpret_cast<Chunk*>(go);
    *reinterpret_cast<Chunk**>(go) = allocChunk;
    allocChunk = reinterpret_cast<Chunk*>(go);
}

GameobjectAllocator::Block* GameobjectAllocator::AllocateBlock() {
    printf("Creating block...\n");
    const size_t blockSize = sizeof(Block::nextBlock) + sizeof(Block::numChunks) + (sizeof(Chunk) * chunksPerBlock);
    printf("Block size: %lu Chunk Size: %lu\n", blockSize, sizeof(Chunk));
    Block* block = reinterpret_cast<Block*>(std::malloc(blockSize));
    printf("Block addr: %p\n", block);

    block->nextBlock = nullptr;
    block->numChunks = chunksPerBlock;
    Chunk* chunks = &block->chunks;

    for (size_t i = 0; i < chunksPerBlock - 1; i++) {
        printf("Getting chunk i = %lu\n", i);
        Chunk& chunk = chunks[i];
        printf("Got chunk adr = %p\n", &(chunks[i]));

        Chunk::Init(chunk, &chunks[i + 1]);
    }

    Chunk::Init(chunks[chunksPerBlock - 1], nullptr);

    return block;
}

