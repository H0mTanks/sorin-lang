#pragma once
#include "Ast.hpp"

Decl* parse_decl();
Typespec* parse_type();
Stmt* parse_stmt();
StmtBlock parse_stmt_block();
Expr* parse_expr();

void parse_test();