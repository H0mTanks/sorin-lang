#include "Resolve.hpp"
#include "Globals.hpp"
#include "StringIntern.hpp"
#include <cassert>

Sym* sym_get(const char* name) {
    for (Sym& it : Global::sym_list) {
        if (it.name == name) {
            return &it;
        }
    }

    return nullptr;
}

void sym_put(Decl* decl) {
    assert(decl->name);
    assert(!sym_get(decl->name));
    Global::sym_list.push_back(Sym{decl->name, decl, SymState::UNRESOLVED});
}

void resolve_test() {
    const char* foo = Global::string_table.add("foo");
    assert(sym_get(foo) == nullptr);
    Decl* decl = decl_const(foo, expr_int(42));
    sym_put(decl);
    Sym* sym = sym_get(foo);
    assert(sym && sym->decl == decl);
}
