#pragma once
#include "StringIntern.hpp"
#include <vector>

void* xcalloc(size_t num_elems, size_t elem_size);

void* xrealloc(void* ptr, size_t num_bytes);

void* xmalloc(size_t num_bytes);

extern StringTable string_table;
extern std::vector<const char*> keywords;


//*global token
struct Token;
extern Token token;

//*codefile stream
extern const char* stream;