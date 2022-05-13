#include <cassert>
#include "Ast.hpp"
#include "Globals.hpp"

Internal void* ast_alloc(size_t size) {
    assert(size != 0);
    void* ptr = Global::ast_arena.alloc(size);
    memset(ptr, 0, size);
    return ptr;
}

void* ast_dup(const void* src, size_t size) {
    if (size == 0) {
        return nullptr;
    }

    void* ptr = Global::ast_arena.alloc(size);
    memcpy(ptr, src, size);
    return ptr;
}

Typespec* typespec_new(TypespecKind kind) {
    Typespec* t = (Typespec*)ast_alloc(sizeof(Typespec));
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
    t->func.args = (Typespec**)ast_dup(args, num_args * sizeof(Typespec*));
    t->func.num_args = num_args;
    t->func.ret = ret;
    return t;
}

Expr* expr_new(ExprKind kind) {
    Expr* e = (Expr*)ast_alloc(sizeof(Expr));
    e->kind = kind;
    return e;
}

Expr* expr_int(i64 int_val) {
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
    e->compound.args = (Expr**)ast_dup(args, num_args * sizeof(Expr*));
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
    e->call.args = (Expr**)ast_dup(args, num_args * sizeof(Expr*));
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

Expr* expr_sizeof_expr(Expr* expr) {
    Expr* e = expr_new(ExprKind::SIZEOF_EXPR);
    e->sizeof_expr = expr;
    return e;
}

Expr* expr_sizeof_type(Typespec* type) {
    Expr* e = expr_new(ExprKind::SIZEOF_TYPE);
    e->sizeof_type = type;
    return e;
}

Stmt* stmt_new(StmtKind kind) {
    Stmt* s = (Stmt*)ast_alloc(sizeof(Stmt));
    s->kind = kind;
    return s;
}

Stmt* stmt_decl(Decl* decl) {
    Stmt* s = stmt_new(StmtKind::DECL);
    s->decl = decl;
    return s;
}

Stmt* stmt_return(Expr* expr) {
    Stmt* s = stmt_new(StmtKind::RETURN);
    s->return_stmt.expr = expr;
    return s;
}

Stmt* stmt_break() {
    return stmt_new(StmtKind::BREAK);
}

Stmt* stmt_continue() {
    return stmt_new(StmtKind::CONTINUE);
}

Stmt* stmt_block(StmtBlock block) {
    Stmt* s = stmt_new(StmtKind::BLOCK);
    s->block = block;
    return s;
}

Stmt* stmt_if(Expr* cond, StmtBlock then_block, ElseIf* elseifs, size_t num_elseifs, StmtBlock else_block) {
    Stmt* s = stmt_new(StmtKind::IF);
    s->if_stmt.cond = cond;
    s->if_stmt.then_block = then_block;
    s->if_stmt.elseifs = (ElseIf*)ast_dup(elseifs, num_elseifs * sizeof(ElseIf));
    s->if_stmt.num_elseifs = num_elseifs;
    s->if_stmt.else_block = else_block;
    return s;
}

Stmt* stmt_while(Expr* cond, StmtBlock block) {
    Stmt* s = stmt_new(StmtKind::WHILE);
    s->while_stmt.cond = cond;
    s->while_stmt.block = block;
    return s;
}

Stmt* stmt_do_while(Expr* cond, StmtBlock block) {
    Stmt* s = stmt_new(StmtKind::DO_WHILE);
    s->while_stmt.cond = cond;
    s->while_stmt.block = block;
    return s;
}

Stmt* stmt_for(Stmt* init, Expr* cond, Stmt* next, StmtBlock block) {
    Stmt* s = stmt_new(StmtKind::FOR);
    s->for_stmt.init = init;
    s->for_stmt.cond = cond;
    s->for_stmt.next = next;
    s->for_stmt.block = block;
    return s;
}

Stmt* stmt_switch(Expr* expr, SwitchCase* cases, size_t num_cases) {
    Stmt* s = stmt_new(StmtKind::SWITCH);
    s->switch_stmt.expr = expr;
    s->switch_stmt.cases = (SwitchCase*)ast_dup(cases, num_cases * sizeof(SwitchCase));
    s->switch_stmt.num_cases = num_cases;
    return s;
}

Stmt* stmt_assign(TokenKind op, Expr* left, Expr* right) {
    Stmt* s = stmt_new(StmtKind::ASSIGN);
    s->assign.op = op;
    s->assign.left = left;
    s->assign.right = right;
    return s;
}

Stmt* stmt_init(const char* name, Expr* expr) {
    Stmt* s = stmt_new(StmtKind::INIT);
    s->init.name = name;
    s->init.expr = expr;
    return s;
}

Stmt* stmt_expr(Expr* expr) {
    Stmt* s = stmt_new(StmtKind::EXPR);
    s->expr = expr;
    return s;
}

Internal Decl* decl_new(DeclKind kind, const char* name) {
    Decl* d = (Decl*)ast_alloc(sizeof(Decl));
    d->kind = kind;
    d->name = name;
    return d;
}

Decl* decl_enum(const char* name, EnumItem* items, size_t num_items) {
    Decl* d = decl_new(DeclKind::ENUM, name);
    d->enum_decl.items = (EnumItem*)ast_dup(items, num_items * sizeof(EnumItem));
    d->enum_decl.num_items = num_items;
    return d;
}

Decl* decl_aggregate(DeclKind kind, const char* name, AggregateItem* items, size_t num_items) {
    assert(kind == DeclKind::STRUCT || kind == DeclKind::UNION);
    Decl* d = decl_new(kind, name);
    d->aggregate.items = (AggregateItem*)ast_dup(items, num_items * sizeof(AggregateItem));
    d->aggregate.num_items = num_items;
    return d;
}


Decl* decl_union(const char* name, AggregateItem* items, size_t num_items) {
    Decl* d = decl_new(DeclKind::UNION, name);
    d->aggregate.items = (AggregateItem*)ast_dup(items, num_items * sizeof(AggregateItem));
    d->aggregate.num_items = num_items;
    return d;
}

Decl* decl_var(const char* name, Typespec* type, Expr* expr) {
    Decl* d = decl_new(DeclKind::VAR, name);
    d->var.type = type;
    d->var.expr = expr;
    return d;
}

Decl* decl_func(const char* name, FuncParam* params, size_t num_params, Typespec* ret_type, StmtBlock block) {
    Decl* d = decl_new(DeclKind::FUNC, name);
    d->func.params = (FuncParam*)ast_dup(params, num_params * sizeof(FuncParam));
    d->func.num_params = num_params;
    d->func.ret_type = ret_type;
    d->func.block = block;
    return d;
}

Decl* decl_const(const char* name, Expr* expr) {
    Decl* d = decl_new(DeclKind::CONST, name);
    d->const_decl.expr = expr;
    return d;
}

Decl* decl_typedef(const char* name, Typespec* type) {
    Decl* d = decl_new(DeclKind::TYPEDEF, name);
    d->typedef_decl.type = type;
    return d;
}