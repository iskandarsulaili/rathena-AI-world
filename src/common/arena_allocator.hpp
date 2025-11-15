#pragma once
// arena_allocator.hpp
// Arena allocator for high-performance entity pools in rAthena core servers

#include <cstddef>
#include <cstdint>
#include <vector>
#include <memory>
#include <mutex>

// Arena allocator for fixed-size objects (thread-safe)
class ArenaAllocator {
public:
    ArenaAllocator(size_t object_size, size_t block_size = 4096);
    ~ArenaAllocator();

    // Allocate a new object
    void* allocate();

    // Reset all allocations (bulk deallocation)
    void reset();

    // Get total allocated objects
    size_t allocated_count() const;

private:
    struct Block {
        std::unique_ptr<uint8_t[]> data;
        size_t used;
    };

    size_t object_size_;
    size_t block_size_;
    std::vector<std::unique_ptr<Block>> blocks_;
    std::mutex mutex_;
};

#endif // ARENA_ALLOCATOR_HPP