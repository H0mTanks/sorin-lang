#pragma once
#include "Ast.hpp"

enum class SymState {
    UNRESOLVED,
    RESOLVING,
    RESOLVED,
};

struct Sym {
    const char* name;
    Decl* decl;
    SymState state;
};

Sym* sym_get(const char* name);

void sym_put(Decl* decl);

void resolve_test();