#include <cassert>
#include "Ast.hpp"
#include "Globals.hpp"

Internal void* ast_alloc(size_t size) {
    assert(size != 0);
    void* ptr = Global::ast_arena.alloc(size);
    memset(ptr, 0, size);
    return ptr;
}

Internal void* ast_dup(const void* src, size_t size) {
    if (size == 0) {
        return nullptr;
    }

    void* ptr = Global::ast_arena.alloc(size);
    memcpy(ptr, src, size);
    return ptr;
}

Internal Typespec* typespec_new(TypespecKind kind) {
    Typespec* t = (Typespec*)Global::ast_arena.alloc(sizeof(Typespec));
    t->kind = kind;
    return t;
}

Internal Typespec* typespec_name(const char* name) {
    Typespec* t = typespec_new(TypespecKind::NAME);
    t->name = name;
    return t;
}

Internal Typespec* typespec_ptr(Typespec* elem) {
    Typespec* t = typespec_new(TypespecKind::PTR);
    t->ptr.elem = elem;
    return t;
}

Internal Typespec* typespec_array(Typespec* elem, Expr* size) {
    Typespec* t = typespec_new(TypespecKind::ARRAY);
    t->array.elem = elem;
    t->array.size = size;
    return t;
}

Internal Typespec* typespec_func(Typespec** args, size_t num_args, Typespec* ret) {
    Typespec* t = typespec_new(TypespecKind::FUNC);
    t->func.args = args;
    t->func.num_args = num_args;
    t->func.ret = ret;
}