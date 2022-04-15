#include <cassert>
#include "StringIntern.hpp"

extern StringTable string_table;

//*checks if the new string is part of the existing list of strings in the intern table
//*if it already exists, return a pointer to the underlying char buffer
//*if it does not exist, allocate memory for it and add it to the intern table.
const char* StringTable::str_intern_range(const char* start, const char* end) {
    size_t len = end - start;

    for (Intern const& intern : interns) {
        if (intern.len == len && strncmp(intern.str, start, len) == 0) {
            return intern.str;
        }
    }

    char* str = (char*)malloc(len + 1);
    memcpy(str, start, len);
    str[len] = 0;
    interns.emplace_back(len, str);

    return str;
}

//*assumes null terminated strings because of strlen, wrapper for str_intern_range
const char* StringTable::str_intern(const char* str) {
    return str_intern_range(str, str + strlen(str));
}

void StringTable::intern_test() {
    char a[] = "hello";
    //*equality by strcmp
    assert(strcmp(a, str_intern(a)) == 0);

    //*idempotence tests
    assert(str_intern(a) == str_intern(a));
    assert(str_intern(str_intern(a)) == str_intern(a));

    char b[] = "hello";
    assert(a != b);
    assert(str_intern(a) == str_intern(b));

    //*prefix test
    char c[] = "hello!";
    assert(str_intern(a) != str_intern(c));

    //*suffix test
    char d[] = "hell";
    assert(str_intern(a) != str_intern(d));

    str_intern("hello there buddy boi");
}