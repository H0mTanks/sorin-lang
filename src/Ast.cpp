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

Typespec* typespec_new(TypespecKind kind) {
    Typespec* t = (Typespec*)Global::ast_arena.alloc(sizeof(Typespec));
    t->kind = kind;
    return t;
}

Typespec* typespec_name(const char* name) {
    Typespec* t = typespec_new(TypespecKind::NAME);
    t->name = name;
    return t;
}

Typespec* typespec_ptr(Typespec* elem) {
    Typespec* t = typespec_new(TypespecKind::PTR);
    t->ptr.elem = elem;
    return t;
}

Typespec* typespec_array(Typespec* elem, Expr* size) {
    Typespec* t = typespec_new(TypespecKind::ARRAY);
    t->array.elem = elem;
    t->array.size = size;
    return t;
}

Typespec* typespec_func(Typespec** args, size_t num_args, Typespec* ret) {
    Typespec* t = typespec_new(TypespecKind::FUNC);
    t->func.args = args;
    t->func.num_args = num_args;
    t->func.ret = ret;
    return t;
}

Expr* expr_new(ExprKind kind) {
    Expr* e = (Expr*)Global::ast_arena.alloc(sizeof(Expr));
    e->kind = kind;
    return e;
}

Expr* expr_int(u64 int_val) {
    Expr* e = expr_new(ExprKind::INT);
    e->int_val = int_val;
    return e;
}

Expr* expr_float(f64 float_val) {
    Expr* e = expr_new(ExprKind::FLOAT);
    e->float_val = float_val;
    return e;
}

Expr* expr_str(const char* str_val) {
    Expr* e = expr_new(ExprKind::STR);
    e->str_val = str_val;
    return e;
}

Expr* expr_name(const char* name) {
    Expr* e = expr_new(ExprKind::NAME);
    e->name = name;
    return e;
}

Expr* expr_compound(Typespec* type, Expr** args, size_t num_args) {
    Expr* e = expr_new(ExprKind::COMPOUND);
    e->compound.type = type;
    e->compound.args = args;
    e->compound.num_args = num_args;
    return e;
}

Expr* expr_cast(Typespec* type, Expr* expr) {
    Expr* e = expr_new(ExprKind::CAST);
    e->cast.type = type;
    e->cast.expr = expr;
    return e;
}

Expr* expr_call(Expr* expr, Expr** args, size_t num_args) {
    Expr* e = expr_new(ExprKind::CALL);
    e->call.expr = expr;
    e->call.args = args;
    e->call.num_args = num_args;
    return e;
}

Expr* expr_index(Expr* expr, Expr* index) {
    Expr* e = expr_new(ExprKind::INDEX);
    e->index.expr = expr;
    e->index.index = index;
    return e;
}

Expr* expr_field(Expr* expr, const char* name) {
    Expr* e = expr_new(ExprKind::FIELD);
    e->field.expr = expr;
    e->field.name = name;
    return e;
}

Expr* expr_unary(TokenKind op, Expr* expr) {
    Expr* e = expr_new(ExprKind::UNARY);
    e->unary.op = op;
    e->unary.expr = expr;
    return e;
}

Expr* expr_binary(TokenKind op, Expr* left, Expr* right) {
    Expr* e = expr_new(ExprKind::BINARY);
    e->binary.op = op;
    e->binary.left = left;
    e->binary.right = right;
    return e;
}

Expr* expr_ternary(Expr* cond, Expr* then_expr, Expr* else_expr) {
    Expr* e = expr_new(ExprKind::TERNARY);
    e->ternary.cond = cond;
    e->ternary.then_expr = then_expr;
    e->ternary.else_expr = else_expr;
    return e;
}