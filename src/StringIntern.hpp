#pragma once

#include <vector>
#include "MemArena.hpp"

//*A string consists of an Intern struct which consists of it's length and pointer to the string.
struct Intern {
    size_t len;
    const char* str;

    Intern(size_t l, const char* s) : len(l), str(s) {}
};

struct StringTable {
    std::vector<Intern> interns;
    Arena arena;

    const char* add_range(const char* start, const char* end);
    const char* add(const char* str);

    void intern_test();
};