#pragma once
#include "StringIntern.hpp"

void* xcalloc(size_t num_elems, size_t elem_size);

void* xrealloc(void* ptr, size_t num_bytes);

void* xmalloc(size_t num_bytes);

extern StringTable string_table;