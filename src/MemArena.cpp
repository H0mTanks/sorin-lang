#include "MemArena.hpp"
#include "Globals.hpp"
#include <memory>
#include <cassert>

Internal constexpr size_t ARENA_ALIGNMENT = 8;
Internal constexpr size_t ARENA_BLOCK_SIZE = 1024;

Internal size_t align_up(size_t num, size_t alignment) {
    size_t modulo = num & (alignment - 1);

    if (modulo) {
        num += alignment - modulo;
    }

    return num;
}

Internal inline size_t max_size(size_t a, size_t b) {
    return a > b ? a : b;
}

void Arena::grow(size_t min_size) {
    size_t size = max_size(ARENA_BLOCK_SIZE, min_size);
    size = align_up(size, ARENA_ALIGNMENT);

    ptr = (u8*)xmalloc(size);
    end = ptr + size;

    blocks.push_back(ptr);
}

void* Arena::alloc(size_t size) {
    if (size > (size_t)(end - ptr)) {
        grow(size);
        assert(size <= (size_t)(end - ptr));
    }

    void* p = ptr;
    ptr = (u8*)align_up((uintptr_t)ptr + size, ARENA_ALIGNMENT);
    assert(ptr <= end);
    return p;
}

void Arena::free_all() {
    for (u8* it : blocks) {
        free(it);
    }
}