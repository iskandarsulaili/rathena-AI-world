// arena_allocator_test.cpp
// Test/verification for ArenaAllocator

#include "arena_allocator.hpp"
#include <cassert>
#include <iostream>
#include <vector>

struct Dummy {
    int a, b, c;
};

void test_arena_allocator() {
    ArenaAllocator arena(sizeof(Dummy), 1024);

    std::vector<Dummy*> ptrs;
    for (int i = 0; i < 100; ++i) {
        Dummy* d = static_cast<Dummy*>(arena.allocate());
        d->a = i;
        d->b = i * 2;
        d->c = i * 3;
        ptrs.push_back(d);
    }

    assert(arena.allocated_count() == 100);

    for (int i = 0; i < 100; ++i) {
        assert(ptrs[i]->a == i);
        assert(ptrs[i]->b == i * 2);
        assert(ptrs[i]->c == i * 3);
    }

    arena.reset();
    assert(arena.allocated_count() == 0);

    std::cout << "[ArenaAllocator] All tests passed." << std::endl;
}

int main() {
    test_arena_allocator();
    return 0;
}