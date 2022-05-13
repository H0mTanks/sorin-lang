#include "Parse.hpp"
#include "Lex.hpp"
#include "Globals.hpp"
#include "Ast.hpp"
#include "Print.hpp"
#include <cassert>
#include <vector>

Internal Typespec* parse_type_func() {
    std::vector<Typespec*> args;
    expect_token(TokenKind::LPAREN);
    
    if (!is_token(TokenKind::RPAREN)) {
        args.push_back(parse_type());
        while (match_token(TokenKind::COMMA)) {
            args.push_back(parse_type());
        }
    }

    expect_token(TokenKind::RPAREN);
    Typespec* ret = nullptr;
    if (match_token(TokenKind::COLON)) {
        ret = parse_type();
    }

    return typespec_func(args.data(), args.size(), ret);
}

Internal Typespec* parse_type_base() {
    if (is_token(TokenKind::NAME)) {
        const char* name = Global::token.name;
        next_token();
        return typespec_name(name);
    }
    else if (match_keyword(Keywords::func_keyword)) {
        return parse_type_func();
    }
    else if (match_token(TokenKind::LPAREN)) {
        Typespec* type = parse_type();
        expect_token(TokenKind::RPAREN);
        return type;
    }

    fatal_syntax_error("Unexpected token %s in type", token_info());
    return nullptr;
}

Typespec* parse_type() {
    Typespec* type = parse_type_base();

    while (is_token(TokenKind::LBRACKET) || is_token(TokenKind::MUL)) {
        if (match_token(TokenKind::LBRACKET)) {
            Expr* expr = nullptr;

            if (!is_token(TokenKind::RBRACKET)) {
                expr = parse_expr();
            }

            expect_token(TokenKind::RBRACKET);
            type = typespec_array(type, expr);
        }
        else {
            assert(is_token(TokenKind::MUL));
            next_token();
            type = typespec_ptr(type);
        }
    }

    return type;
}

Internal Expr* parse_expr_compound(Typespec* type) {
    expect_token(TokenKind::LBRACE);
    std::vector<Expr*> args;

    if (!is_token(TokenKind::RBRACE)) {
        args.push_back(parse_expr());
        while (match_token(TokenKind::COMMA)) {
            args.push_back(parse_expr());
        }
    }

    expect_token(TokenKind::RBRACE);
    return expr_compound(type, args.data(), args.size());
}

Internal Expr* parse_expr_operand() {
    if (is_token(TokenKind::INT)) {
        i64 val = Global::token.int_val;
        next_token();
        return expr_int(val);
    }
    else if (is_token(TokenKind::FLOAT)) {
        f64 val = Global::token.float_val;
        next_token();
        return expr_float(val);
    }
    else if (is_token(TokenKind::STR)) {
        const char* val = Global::token.str_val;
        next_token();
        return expr_str(val);
    }
    else if (is_token(TokenKind::NAME)) {
        const char* name = Global::token.name;
        next_token();
        if (is_token(TokenKind::LBRACE)) {
            return parse_expr_compound(typespec_name(name));
        }
        else {
            return expr_name(name);
        }
    }
    else if (match_keyword(Keywords::sizeof_keyword)) {
        expect_token(TokenKind::LPAREN);
        if (match_token(TokenKind::COLON)) {
            Typespec* type = parse_type();
            expect_token(TokenKind::RPAREN);
            return expr_sizeof_type(type);
        }
        else {
            Expr* expr = parse_expr();
            expect_token(TokenKind::RPAREN);
            return expr_sizeof_expr(expr);
        }
    }
    else if (is_token(TokenKind::LBRACE)) {
        return parse_expr_compound(nullptr);
    }
    else if (match_token(TokenKind::LPAREN)) {
        if (match_token(TokenKind::COLON)) {
            Typespec* type = parse_type();
            expect_token(TokenKind::RPAREN);
            return parse_expr_compound(type);
        }
        else {
            Expr* expr = parse_expr();
            expect_token(TokenKind::RPAREN);
            return expr;
        }
    }

    fatal_syntax_error("Unexpected token %s in expression", token_info());
    return nullptr;
}

Internal Expr* parse_expr_base() {
    Expr* expr = parse_expr_operand();
    while (is_token(TokenKind::LPAREN) || is_token(TokenKind::LBRACKET) || is_token(TokenKind::DOT)) {
        if (match_token(TokenKind::LPAREN)) {
            std::vector<Expr*> args;
            if (!is_token(TokenKind::RPAREN)) {
                args.push_back(parse_expr());
                while (match_token(TokenKind::COMMA)) {
                    args.push_back(parse_expr());
                }
            }

            expect_token(TokenKind::RPAREN);
            expr = expr_call(expr, args.data(), args.size());
        }
        else if (match_token(TokenKind::LBRACKET)) {
            Expr* index = parse_expr();
            expect_token(TokenKind::RBRACKET);
            expr = expr_index(expr, index);
        }
        else {
            assert(is_token(TokenKind::DOT));
            next_token();
            const char* field = Global::token.name;
            expect_token(TokenKind::NAME);
            expr = expr_field(expr, field);
        }
    }

    return expr;
}

Internal bool is_unary_op() {
    return is_token(TokenKind::ADD) || is_token(TokenKind::SUB) || is_token(TokenKind::MUL) || is_token(TokenKind::AND);
}

Internal Expr* parse_expr_unary() {
    if (is_unary_op()) {
        TokenKind op = Global::token.kind;
        next_token();
        return expr_unary(op, parse_expr_unary());
    }

    return parse_expr_base();
}

Internal bool is_mul_op() {
    return TokenKind::FIRST_MUL <= Global::token.kind && Global::token.kind <= TokenKind::LAST_MUL;
}

Internal Expr* parse_expr_mul() {
    Expr* expr = parse_expr_unary();
    while (is_mul_op()) {
        TokenKind op = Global::token.kind;
        next_token();
        expr = expr_binary(op, expr, parse_expr_unary());
    }

    return expr;
}

Internal bool is_add_op() {
    return TokenKind::FIRST_ADD <= Global::token.kind && Global::token.kind <= TokenKind::LAST_ADD;
}

Internal Expr* parse_expr_add() {
    Expr* expr = parse_expr_mul();
    while (is_add_op()) {
        TokenKind op = Global::token.kind;
        next_token();
        expr = expr_binary(op, expr, parse_expr_mul());
    }

    return expr;
}

Internal bool is_cmp_op() {
    return TokenKind::FIRST_CMP <= Global::token.kind && Global::token.kind <= TokenKind::LAST_CMP;
}

Internal Expr* parse_expr_cmp() {
    Expr* expr = parse_expr_add();
    while (is_cmp_op()) {
        TokenKind op = Global::token.kind;
        next_token();
        expr =  expr_binary(op, expr, parse_expr_add());
    }

    return expr;
}

Internal Expr* parse_expr_and() {
    Expr* expr = parse_expr_cmp();
    while (match_token(TokenKind::AND_AND)) {
        expr = expr_binary(TokenKind::AND_AND, expr, parse_expr_cmp());
    }
    return expr;
}

Internal Expr* parse_expr_or() {
    Expr* expr = parse_expr_and();
    while (match_token(TokenKind::OR_OR)) {
        expr = expr_binary(TokenKind::OR_OR, expr, parse_expr_and());
    }
    return expr;
}

Internal Expr* parse_expr_ternary() {
    Expr* expr = parse_expr_or();
    if (match_token(TokenKind::QUESTION)) {
        Expr* then_expr = parse_expr_ternary();
        expect_token(TokenKind::COLON);
        Expr* else_expr = parse_expr_ternary();
        expr = expr_ternary(expr, then_expr, else_expr);
    }

    return expr;
}

Expr* parse_expr() {
    return parse_expr_ternary();
}

Internal Expr* parse_paren_expr() {
    expect_token(TokenKind::LPAREN);
    Expr* expr = parse_expr();
    expect_token(TokenKind::RPAREN);
    return expr;
}

const char* parse_name() {
    const char* name = Global::token.name;
    expect_token(TokenKind::NAME);
    return name;
}

Internal EnumItem parse_decl_enum_item() {
    const char* name = parse_name();

    Expr* init = nullptr;
    if (match_token(TokenKind::ASSIGN)) {
        init = parse_expr();
    }

    return EnumItem{name, init};
}

Internal Decl* parse_decl_enum() {
    const char* name = parse_name();
    
    expect_token(TokenKind::LBRACE);

    std::vector<EnumItem> items;
    if (!is_token(TokenKind::RBRACE)) {
        items.push_back(parse_decl_enum_item());
        while (match_token(TokenKind::COMMA)) {
            items.push_back(parse_decl_enum_item());
        }
    }

    expect_token(TokenKind::RBRACE);
    return decl_enum(name, items.data(), items.size());
}

Internal AggregateItem parse_decl_aggregate_item() {
    std::vector<const char*> names;
    names.push_back(parse_name());

    while (match_token(TokenKind::COMMA)) {
        names.push_back(parse_name());
    }

    expect_token(TokenKind::COLON);
    Typespec* type = parse_type();
    
    expect_token(TokenKind::SEMICOLON);
    return AggregateItem{ (const char**)ast_dup(names.data(), names.size() * sizeof(const char*)), names.size(), type }; //?see if this ast_dup call can be pulled out into a func
}

Internal Decl* parse_decl_aggregate(DeclKind kind) {
    assert(kind == DeclKind::STRUCT || kind == DeclKind::UNION);
    const char* name = parse_name();
    expect_token(TokenKind::LBRACE);

    std::vector<AggregateItem> items;
    while (!is_token_eof() && !is_token(TokenKind::RBRACE)) {
        items.push_back(parse_decl_aggregate_item());
    }
    expect_token(TokenKind::RBRACE);

    return decl_aggregate(kind, name, items.data(), items.size());
}

Internal Decl* parse_decl_var() {
    const char* name = parse_name();

    if (match_token(TokenKind::ASSIGN)) {
        return decl_var(name, nullptr, parse_expr());
    }
    else if (match_token(TokenKind::COLON)) {
        Typespec* type = parse_type();
        Expr* expr = nullptr;
        if (match_token(TokenKind::ASSIGN)) {
            expr = parse_expr();
        }
        return decl_var(name, type, expr);
    }

    fatal_syntax_error("Expected TokenKind::COLON or '=' after var, got %s", token_info());
    return nullptr;
}

Internal Decl* parse_decl_const() {
    const char* name = parse_name();
    expect_token(TokenKind::ASSIGN);
    return decl_const(name, parse_expr());
}

Internal Decl* parse_decl_typedef() {
    const char* name = parse_name();
    expect_token(TokenKind::ASSIGN);
    return decl_typedef(name, parse_type());
}

Internal Stmt* parse_stmt_if() {
    Expr* cond = parse_paren_expr();
    StmtBlock then_block = parse_stmt_block();
    StmtBlock else_block = {};
    std::vector<ElseIf> elseifs;

    while (match_keyword(Keywords::else_keyword)) {
        if (!match_keyword(Keywords::if_keyword)) {
            else_block = parse_stmt_block();
            break;
        }
        Expr* elseif_cond = parse_paren_expr();
        StmtBlock elseif_block = parse_stmt_block();
        elseifs.push_back(ElseIf{elseif_cond, elseif_block});
    }

    return stmt_if(cond, then_block, elseifs.data(), elseifs.size(), else_block);
}

Stmt* parse_stmt_while() {
    Expr* cond = parse_paren_expr();
    return stmt_while(cond, parse_stmt_block());
}

Stmt* parse_stmt_do_while() {
    StmtBlock block = parse_stmt_block();
    if (!match_keyword(Keywords::while_keyword)) {
        fatal_syntax_error("Expected 'while' after 'do' block");
        return nullptr;
    }

    Expr* cond = parse_paren_expr();
    Stmt* stmt = stmt_do_while(cond, block);
    expect_token(TokenKind::SEMICOLON);
    return stmt;
}

Internal bool is_assign_op() {
    return TokenKind::FIRST_ASSIGN <= Global::token.kind && Global::token.kind <= TokenKind::LAST_ASSIGN;
}

Internal Stmt* parse_simple_stmt() {
    Expr* expr = parse_expr();
    Stmt* stmt = nullptr;

    if (match_token(TokenKind::COLON_ASSIGN)) {
        if (expr->kind != ExprKind::NAME) {
            fatal_syntax_error("Colon Assign must be preceded by name");
            return nullptr;
        }
        stmt = stmt_init(expr->name, parse_expr());
    }
    else if (is_assign_op()) {
        TokenKind op = Global::token.kind;
        next_token();
        stmt = stmt_assign(op, expr, parse_expr());
    }
    else if (is_token(TokenKind::INC) || is_token(TokenKind::DEC)) {
        TokenKind op = Global::token.kind;
        next_token();
        stmt = stmt_assign(op, expr, nullptr);
    }
    else {
        stmt = stmt_expr(expr); //?what
    }

    return stmt;
}

Internal Stmt* parse_stmt_for() {
    expect_token(TokenKind::LPAREN);
    Stmt* init = nullptr;
    if (!is_token(TokenKind::SEMICOLON)) {
        init = parse_simple_stmt();
    }
    expect_token(TokenKind::SEMICOLON);

    Expr* cond = nullptr;
    if (!is_token(TokenKind::SEMICOLON)) {
        cond = parse_expr();
    }
    expect_token(TokenKind::SEMICOLON);

    Stmt* next = nullptr;
    if (!is_token(TokenKind::SEMICOLON)) {
        next = parse_simple_stmt();
        if (next->kind == StmtKind::INIT) {
            syntax_error("Init statements not allowed in for-statement's next clause");
        }
    }
    expect_token(TokenKind::RPAREN);

    return stmt_for(init, cond, next, parse_stmt_block());
}


Internal SwitchCase parse_stmt_switch_case() {
    std::vector<Expr*> exprs;
    bool is_default = false;

    while (is_keyword(Keywords::case_keyword) || is_keyword(Keywords::default_keyword)) {
        if (match_keyword(Keywords::case_keyword)) {
            exprs.push_back(parse_expr());
        }
        else {
            assert(is_keyword(Keywords::default_keyword));
            next_token();
            if (is_default) {
                syntax_error("Duplicate default labels in the same switch clause");
            }
            is_default = true;
        }
    }

    expect_token(TokenKind::COLON);

    std::vector<Stmt*> stmts;
    while (!is_token_eof() && !is_token(TokenKind::RBRACE) && !is_keyword(Keywords::case_keyword) && !is_keyword(Keywords::default_keyword)) {
        stmts.push_back(parse_stmt());
    }

    StmtBlock block = { (Stmt**)ast_dup(stmts.data(), stmts.size() * sizeof(Stmt*)), stmts.size() }; //?see if this ast_dup call can be pulled out into a func
    return SwitchCase{ (Expr**)ast_dup(exprs.data(), exprs.size() * sizeof(Expr*)), exprs.size(), is_default, block };//?see if this ast_dup call can be pulled out into a func
}


Internal Stmt* parse_stmt_switch() {
    Expr* expr = parse_paren_expr();
    std::vector<SwitchCase> cases;
    expect_token(TokenKind::LBRACE);

    while (!is_token_eof() && !is_token(TokenKind::RBRACE)) {
        cases.push_back(parse_stmt_switch_case());
    }
    expect_token(TokenKind::RBRACE);

    return stmt_switch(expr, cases.data(), cases.size());
}


Stmt* parse_stmt() {
    using namespace Keywords;
    if (match_keyword(if_keyword)) {
        return parse_stmt_if();
    }
    else if (match_keyword(while_keyword)) {
        return parse_stmt_while();
    }
    else if (match_keyword(do_keyword)) {
        return parse_stmt_do_while();
    }
    else if (match_keyword(for_keyword)) {
        return parse_stmt_for();
    }
    else if (match_keyword(switch_keyword)) {
        return parse_stmt_switch();
    }
    else if (is_token(TokenKind::LBRACE)) {
        return stmt_block(parse_stmt_block());
    }
    else if (match_keyword(break_keyword)) {
        expect_token(TokenKind::SEMICOLON);
        return stmt_break();
    }
    else if (match_keyword(continue_keyword)) {
        expect_token(TokenKind::SEMICOLON);
        return stmt_continue();
    }
    else if (match_keyword(return_keyword)) {
        Expr* expr = nullptr;
        if (!is_token(TokenKind::SEMICOLON)) {
            expr = parse_expr();
        }

        expect_token(TokenKind::SEMICOLON);
        return stmt_return(expr);
    }

    Decl* decl = parse_decl_opt();
    if (decl) {
        return stmt_decl(decl);
    }

    Stmt* stmt = parse_simple_stmt();
    expect_token(TokenKind::SEMICOLON);
    return stmt;
}

StmtBlock parse_stmt_block() {
    expect_token(TokenKind::LBRACE);
    std::vector<Stmt*> stmts;
    while (!is_token_eof() && !is_token(TokenKind::RBRACE)) {
        stmts.push_back(parse_stmt());
    }

    expect_token(TokenKind::RBRACE);
    return StmtBlock{(Stmt**)ast_dup(stmts.data(), stmts.size() * sizeof(Stmt*)), stmts.size()}; //?see if this ast_dup call can be pulled out into a func
}

Internal FuncParam parse_decl_func_param() {
    const char* name = parse_name();
    expect_token(TokenKind::COLON);
    return FuncParam{name, parse_type()};
}

Internal Decl* parse_decl_func() {
    const char* name = parse_name();
    expect_token(TokenKind::LPAREN);

    std::vector<FuncParam> params;
    if (!is_token(TokenKind::RPAREN)) {
        params.push_back(parse_decl_func_param());
        while (match_token(TokenKind::COMMA)) {
            params.push_back(parse_decl_func_param());
        }
    }

    expect_token(TokenKind::RPAREN);
    Typespec* ret_type = nullptr;
    if (match_token(TokenKind::COLON)) {
        ret_type = parse_type();
    }

    StmtBlock block = parse_stmt_block();
    return decl_func(name, params.data(), params.size(), ret_type, block);

}

Decl* parse_decl_opt() {
    using namespace Keywords;
    if (match_keyword(enum_keyword)) {
        return parse_decl_enum();
    }
    else if (match_keyword(struct_keyword)) {
        return parse_decl_aggregate(DeclKind::STRUCT);
    }
    else if (match_keyword(union_keyword)) {
        return parse_decl_aggregate(DeclKind::UNION);
    }
    else if (match_keyword(var_keyword)) {
        return parse_decl_var();
    }
    else if (match_keyword(const_keyword)) {
        return parse_decl_const();
    }
    else if (match_keyword(typedef_keyword)) {
        return parse_decl_typedef();
    }
    else if (match_keyword(func_keyword)) {
        return parse_decl_func();
    }

    return nullptr;
}

Decl* parse_decl() {
    Decl* decl = parse_decl_opt();
    if (!decl) {
        fatal_syntax_error("Expected declaration keyword, got %s", token_info());
    }
    return decl;
}

void parse_and_print_decl(const char* str) {
    init_stream(str);
    Decl* decl = parse_decl();
    print_decl(decl);
    printf("\n\n");
}

void parse_test() {
    const char* tests[] = {
        "const n = sizeof(:int*[16])",
        "const n = sizeof(1+2)",
        "var x = b == 1 ? 1+2 : 3-4",
        "func fact(n: int): int { trace(\"fact\"); if (n == 0) { return 1; } else { return n * fact(n-1); } }",
        "func fact(n: int): int { p := 1; for (i := 1; i <= n; i++) { p *= i; } return p; }",
        "var foo = a ? a&b + c<<d + e*f == +u-v-w + *g/h(x,y) + -i%k[x] && m <= n*(p+q)/r : 0",
        "func f(x: int): bool { switch(x) { case 0: case 1: return true; case 2: default: return false; } }",
        "enum Color { RED = 3, GREEN, BLUE = 0 }",
        "const pi = 3.14",
        "struct Vector { x, y: float; }",
        "var v = Vector{1.0, -1.0}",
        "var v: Vector = {1.0, -1.0}",
        "union IntOrFloat { i: int; f: float; }",
        "typedef Vectors = Vector[1+2]",
        "func f() { do { print(42); } while(1); }",
        "typedef T = (func(int):int)[16]",
        "func f() { enum E { A, B, C } return; }",
        "func f() { if (1) { return 1; } else if (2) { return 2; } else { return 3; } }",
    };

    for (const char** it = tests; it != tests + sizeof(tests) / sizeof(*tests); it++) {
        parse_and_print_decl(*it);
    }
}