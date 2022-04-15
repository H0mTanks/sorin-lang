#pragma once

#include <vector>
#include <types.hpp>

struct Arena {
    u8* ptr;
    u8* end;
    std::vector<u8*> blocks;

    void grow(size_t min_size);
    void* alloc(size_t size);
    void free_all();
};