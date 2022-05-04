#include <cstdarg>
#include "Globals.hpp"
#include "StringIntern.hpp"
#include "Lex.hpp"

void* xcalloc(size_t num_elems, size_t elem_size) {
    void* ptr = calloc(num_elems, elem_size);
    if (!ptr) {
        perror("xcalloc failed");
        exit(1);
    }
    return ptr;
}

void* xrealloc(void* ptr, size_t num_bytes) {
    ptr = realloc(ptr, num_bytes);
    if (!ptr) {
        perror("xrealloc failed");
        exit(1);
    }
    return ptr;
}

void* xmalloc(size_t num_bytes) {
    void* ptr = malloc(num_bytes);
    if (!ptr) {
        perror("xmalloc failed");
        exit(1);
    }
    return ptr;
}

void fatal(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf("FATAL: ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    exit(1);
}

namespace Global {

StringTable string_table;
std::vector<const char*> keywords;

Token token;
std::vector<const char*> token_kind_names;

const char* stream = nullptr;

}