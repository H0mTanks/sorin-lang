#include <cassert>
#include "StringIntern.hpp"
#include "MemArena.hpp"

extern StringTable string_table;

//*checks if the new string is part of the existing list of strings in the intern table
//*if it already exists, return a pointer to the underlying char buffer
//*if it does not exist, allocate memory for it and add it to the intern table.
const char* StringTable::add_range(const char* start, const char* end) {
    size_t len = end - start;

    for (Intern const& intern : interns) {
        if (intern.len == len && strncmp(intern.str, start, len) == 0) {
            return intern.str;
        }
    }

    char* str = (char*)arena.alloc(len + 1);
    memcpy(str, start, len);
    str[len] = 0;
    interns.emplace_back(len, str);

    return str;
}

//*assumes null terminated strings because of strlen, wrapper for str_intern_range
const char* StringTable::add(const char* str) {
    return add_range(str, str + strlen(str));
}

void StringTable::intern_test() {
    char a[] = "hello";
    //*equality by strcmp
    assert(strcmp(a, add(a)) == 0);

    //*idempotence tests
    assert(add(a) == add(a));
    assert(add(add(a)) == add(a));

    char b[] = "hello";
    assert(a != b);
    assert(add(a) == add(b));

    //*prefix test
    char c[] = "hello!";
    assert(add(a) != add(c));

    //*suffix test
    char d[] = "hell";
    assert(add(a) != add(d));

    add("hello there buddy boi");
}