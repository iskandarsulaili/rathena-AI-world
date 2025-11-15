// arena_allocator.cpp
// Implementation of ArenaAllocator for rAthena core servers

#include "arena_allocator.hpp"
#include <cassert>
#include <cstring>

ArenaAllocator::ArenaAllocator(size_t object_size, size_t block_size)
    : object_size_(object_size), block_size_(block_size) {}

ArenaAllocator::~ArenaAllocator() {
    reset();
}

void* ArenaAllocator::allocate() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (blocks_.empty() || blocks_.back()->used + object_size_ > block_size_) {
        // Allocate new block
        auto block = std::make_unique<Block>();
        block->data = std::make_unique<uint8_t[]>(block_size_);
        block->used = 0;
        blocks_.push_back(std::move(block));
    }
    Block* block = blocks_.back().get();
    void* ptr = block->data.get() + block->used;
    block->used += object_size_;
    return ptr;
}

void ArenaAllocator::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    blocks_.clear();
}

size_t ArenaAllocator::allocated_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t count = 0;
    for (const auto& block : blocks_) {
        count += block->used / object_size_;
    }
    return count;
}