#pragma once
#include <vector>
#include "Lex.hpp"
#include "StringIntern.hpp"

void* xcalloc(size_t num_elems, size_t elem_size);

void* xrealloc(void* ptr, size_t num_bytes);

void* xmalloc(size_t num_bytes);


namespace Global {

extern StringTable string_table;
extern std::vector<const char*> keywords;

//*global token
extern Token token;

extern std::vector<const char*> token_kind_names;

//*codefile stream
extern const char* stream;

}