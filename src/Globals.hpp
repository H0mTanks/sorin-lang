#pragma once
#include <vector>
#include "Lex.hpp"
#include "StringIntern.hpp"

#define KILOBYTE(x) 1024 * (x)
#define MEGABYTE(x) 1024 * KILOBYTE(x)
#define GIGABYTE(x) 1024 * MEGABYTE(x)

void* xcalloc(size_t num_elems, size_t elem_size);

void* xrealloc(void* ptr, size_t num_bytes);

void* xmalloc(size_t num_bytes);

void fatal(const char* fmt, ...);

void syntax_error(const char* fmt, ...);

void fatal_syntax_error(const char* fmt, ...);

namespace Global {

extern StringTable string_table;
extern std::vector<const char*> keywords;

//*global token
extern Token token;

extern std::vector<const char*> token_kind_names;

//*codefile stream
extern const char* stream;

//*memory for ast
extern Arena ast_arena;

}