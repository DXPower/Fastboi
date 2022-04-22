#include "GameobjectAllocator.h"
#include "FastboiCore.h"
#include "Gameobject.h"
#include <vector>

using namespace Fastboi;

namespace Fastboi {
    extern GameobjectAllocator gameobjectAllocator;
}

GameobjectAllocator Fastboi::gameobjectAllocator = GameobjectAllocator();

struct GameobjectAllocator::Chunk {
    Gameobject allocation; // Must keep this as first member
    void* nextStart; // Used to keep track of all gameobjects that need to be started. Equal to address of itself if started.

    static void Init(Chunk& chunk, Chunk* alloc) {
        std::memset((void*) &chunk, 0, sizeof(Chunk));

        *reinterpret_cast<Chunk**>(&chunk.allocation) = alloc; // First memory in the chunk is now the allocation pointer to the next chunk
        chunk.nextStart = nullptr;
    }

    bool IsStarted() const {
        return nextStart == &nextStart;
    }

    void MarkStarted() {
        nextStart = &nextStart;
    }
};

// Used only for defining the locations of variables inside the malloced creation in AllocateBlock
#pragma pack(push, 1)
struct GameobjectAllocator::Block {
    Block* nextBlock;
    size_t numChunks;
    Chunk chunks; // Chunks begin immediately here in memory.

    constexpr static std::size_t GetMemBaseSize() {
        return sizeof(nextBlock) + sizeof(chunks);
    }

    static std::size_t  GetMemSize(std::size_t chunksPerBlock) {
        return GetMemBaseSize() + (sizeof(Chunk) * chunksPerBlock);
    }
};
#pragma pack(pop)

GameobjectAllocator::GameobjectAllocator() {
    const std::size_t blockTotSize = Fastboi::GetCacheSize() / 4;
    const std::size_t chunksSpace = blockTotSize - Block::GetMemBaseSize();
    
    chunksPerBlock = chunksSpace / sizeof(Chunk);
    printf("Chunks per block: %lu\n", chunksPerBlock);
}

void* GameobjectAllocator::Allocate() {
    if (allocChunk == nullptr) {
        allocChunk = &AllocateBlock()->chunks;
    }

    void* ret = static_cast<void*>(allocChunk);
    allocChunk = *reinterpret_cast<Chunk**>(ret);

    MarkUnstarted(*reinterpret_cast<Chunk*>(ret));

    return ret;
}

void GameobjectAllocator::Deallocate(void* go) {
    Chunk* const prev = allocChunk;
    allocChunk = reinterpret_cast<Chunk*>(go);
    
    // The gameobject is started if the nextStart field points to itself
    if (!allocChunk->IsStarted()) {
        Application::ThrowRuntimeException("Attempt to deallocate unstarted gameobject", Application::DEALLOCATE_UNSTARTED_GO);
    }

    Chunk::Init(*allocChunk, prev);
}

void GameobjectAllocator::MarkUnstarted(Chunk& chunk) {
    chunk.nextStart = nullptr;

    if (unstartedHead == nullptr) {
        unstartedHead = &chunk;
        unstartedTail = &chunk;
    } else {
        unstartedTail->nextStart = &chunk;
        unstartedTail = &chunk;
    }

}

void GameobjectAllocator::StartAll() {
    Chunk* next;

    while (unstartedHead != nullptr) {
        reinterpret_cast<Gameobject*>(unstartedHead)->Start();

        next = reinterpret_cast<Chunk*>(unstartedHead->nextStart);

        unstartedHead->MarkStarted();
        unstartedHead = next;
    }

    unstartedTail = nullptr;
}

GameobjectAllocator::Block* GameobjectAllocator::AllocateBlock() {
    Block* block = reinterpret_cast<Block*>(std::malloc(Block::GetMemSize(chunksPerBlock)));

    if (blockHead != nullptr) {
        blockTail->nextBlock = block;
        blockTail = block;
    } else {
        blockHead = block;
        blockTail = block;
    }

    block->nextBlock = nullptr;
    block->numChunks = chunksPerBlock;
    Chunk* chunks = &block->chunks;

    for (size_t i = 0; i < chunksPerBlock - 1; i++) {
        Chunk& chunk = chunks[i];

        Chunk::Init(chunk, &chunks[i + 1]);
    }

    Chunk::Init(chunks[chunksPerBlock - 1], nullptr);

    // printf("Size of block: %lu bytes\n", Block::GetMemSize(chunksPerBlock));
    return block;
}

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

GameobjectAllocator::iterator GameobjectAllocator::iterator::operator++(int _) {
    if (unlikely(block == nullptr)) return *this;

    if (likely(cn != block->numChunks - 1)) {
        cn++;
    } else {
        block = block->nextBlock;
        cn = 0;
    }

    return *this;
}

Gameobject& GameobjectAllocator::iterator::operator*() const {
    Chunk* chunks = &block->chunks; // The chunks of the block are contiguous starting at the chunks variable.
    return reinterpret_cast<Gameobject&>(chunks[cn]);
}

#undef likely
#undef unlikely
