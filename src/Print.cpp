#include "Print.hpp"
#include <cassert>

GlobalVariable int indent;

//*print a newline followed by appropriate amount of indent
Internal void print_newline() {
    printf("\n%.*s", 2 * indent, "                                                                                  ");
}

void print_typespec(Typespec* type) {
    Typespec* t = type;
    switch (t->kind) {
        case TypespecKind::NAME: {
            printf("%s", t->name);
            break;
        }
        case TypespecKind::FUNC: {
            printf("(func (");

            for (Typespec** it = t->func.args; it != t->func.args + t->func.num_args; it++) {
                printf(" ");
                print_typespec(*it);
            }

            printf(") ");
            print_typespec(t->func.ret);
            printf(")");
            break;
        }
        case TypespecKind::ARRAY: {
            printf("(array ");
            print_typespec(t->array.elem);
            printf(" ");
            //!TODO! print_expr(array.size);
            printf(")");
            break;
        }
        case TypespecKind::PTR: {
            printf("(ptr ");
            print_typespec(t->ptr.elem);
            printf(")");
            break;
        }
        default: {
            assert(false);
            break;
        }
    }
}

void print_expr(Expr* expr) {
    Expr* e = expr;
    switch (e->kind) {
        case ExprKind::INT: {
            printf("%llu", e->int_val);
            break;
        }
        case ExprKind::FLOAT: {
            printf("%f", e->float_val);
            break;
        }
        case ExprKind::STR: {
            printf("\"%s\"", e->str_val);
            break;
        }
        case ExprKind::NAME: {
            printf("%s", e->name);
            break;
        }
        case ExprKind::CAST: {
            printf("(cast ");
            print_typespec(e->cast.type);
            printf(" ");
            print_expr(e->cast.expr);
            printf(")");
            break;
        }
        case ExprKind::CALL: {
            printf("(");
            print_expr(e->call.expr);

            for (Expr** it = e->call.args; it != e->call.args + e->call.num_args; it++) {
                printf(" ");
                print_expr(*it);
            }

            printf(")");
            break;
        }
        case ExprKind::INDEX: {
            printf("(index ");
            print_expr(e->index.expr);
            printf(" ");
            print_expr(e->index.index);
            printf(")");
            break;
        }
        case ExprKind::FIELD: {
            printf("(field ");
            print_expr(e->field.expr);
            printf(" %s)", e->field.name);
            break;
        }
        case ExprKind::COMPOUND: {
            printf("(compound ");
            if (e->compound.type) {
                print_typespec(e->compound.type);
            }
            else {
                printf("nil");
            }

            for (Expr** it = e->compound.args; it != e->compound.args + e->compound.num_args; it++) {
                printf(" ");
                print_expr(*it);
            }

            printf(")");
            break;
        }
        case ExprKind::UNARY: {
            printf("(%s ", temp_token_kind_str(e->unary.op));
            print_expr(e->unary.expr);
            printf(")");
            break;
        }
        case ExprKind::BINARY: {
            printf("(%s ", temp_token_kind_str(e->binary.op));
            print_expr(e->binary.left);
            printf(" ");
            print_expr(e->binary.right);
            printf(")");
            break;
        }
        case ExprKind::TERNARY: {
            printf("(? ");
            print_expr(e->ternary.cond);
            printf(" ");
            print_expr(e->ternary.then_expr);
            printf(" ");
            print_expr(e->ternary.else_expr);
            printf(")");
            break;
        }
        default: {
            assert(false);
            break;
        }
    }
}

void print_stmt_block(StmtBlock block) {
    printf("(block");
    indent++;

    for (Stmt** it = block.stmts; it != block.stmts + block.num_stmts; it++) {
        print_stmt(*it);
    }

    indent--;
    printf(")");
}

void print_stmt(Stmt* stmt) {
    Stmt* s = stmt;
    switch (s->kind) {
        case StmtKind::RETURN: {
            printf("(return ");
            print_expr(s->return_stmt.expr);
            printf(")");
            break;
        }
        case StmtKind::BREAK: {
            printf("(break)");
            break;
        }
        case StmtKind::CONTINUE: {
            printf("(continue)");
            break;
        }
        case StmtKind::BLOCK: {
            print_stmt_block(s->block);
            break;
        }
        case StmtKind::IF: {
            printf("(if ");
            print_expr(s->if_stmt.cond);
            indent++;
            print_newline();
            print_stmt_block(s->if_stmt.then_block);

            for (ElseIf* it = s->if_stmt.elseifs; it != s->if_stmt.elseifs + s->if_stmt.num_elseifs; it++) {
                print_newline();
                printf("elseif ");
                print_expr(it->cond);
                print_newline();
                print_stmt_block(it->block);
            }

            if (s->if_stmt.else_block.num_stmts != 0) {
                print_newline();
                printf("else ");
                print_newline();
                print_stmt_block(s->if_stmt.else_block);
            }

            indent--;
            printf(")");
            break;
        }
        case StmtKind::WHILE: {
            printf("(while ");
            print_expr(s->while_stmt.cond);
            indent++;
            print_newline();
            print_stmt_block(s->while_stmt.block);
            indent--;
            printf(")");
            break;
        }
        case StmtKind::DO_WHILE: {
            printf("(do-while ");
            print_expr(s->while_stmt.cond);
            indent++;
            print_newline();
            print_stmt_block(s->while_stmt.block);
            indent--;
            printf(")");
            break;
        }
        case StmtKind::FOR: {
            printf("(for ");
            print_stmt(s->for_stmt.init);
            print_expr(s->for_stmt.cond);
            print_stmt(s->for_stmt.next);
            indent++;
            print_newline();
            print_stmt_block(s->for_stmt.block);
            indent--;
            printf(")");
            break;
        }
        case StmtKind::SWITCH: {
            printf("(switch ");
            print_expr(s->switch_stmt.expr);
            indent++;

            for (SwitchCase* it = s->switch_stmt.cases; it != s->switch_stmt.cases + s->switch_stmt.num_cases; it++) {
                print_newline();
                printf("(case (%s", it->is_default ? " default" : "");

                for (Expr** expr = it->exprs; expr != it->exprs + it->num_exprs; expr++) {
                    printf(" ");
                    print_expr(*expr);
                }
                printf(") ");
                indent++;
                print_newline();
                print_stmt_block(it->block);
                indent--;
            }
            indent--;
            printf(")");
            break;
        }
        case StmtKind::ASSIGN: {
            printf("(%s ", temp_token_kind_str(s->assign.op));//?different in sorin_old
            print_expr(s->assign.left);
            if (s->assign.right) {
                printf(" ");
                print_expr(s->assign.right);
            }
            printf(")");
            break;
        }
        case StmtKind::INIT: {
            printf("(:= %s ", s->init.name);
            print_expr(s->init.expr);
            printf(")");
            break;
        }
        case StmtKind::EXPR: {
            print_expr(s->expr);
            break;
        }
        default: {
            assert(0);
            break;
        }
    }
}

template<typename T>
T** list(const std::initializer_list<T*>& lst) {
    return const_cast<T**>(lst.begin());
}

template<typename T>
T* list_single(const std::initializer_list<T>& lst) {
    return const_cast<T*>(lst.begin());
}

void print_tests() {
    //*Expressions
    Expr* exprs[] = {
        expr_binary((TokenKind)'+', expr_int(1), expr_int(2)),
        expr_unary((TokenKind)'-', expr_float(3.14)),
        expr_ternary(expr_name("flag"), expr_str("true"), expr_str("false")),
        expr_field(expr_name("person"), "name"),
        expr_call(expr_name("fact"), list({ expr_int(42) }), 1),
        expr_index(expr_field(expr_name("person"), "siblings"), expr_int(3)),
        expr_cast(typespec_ptr(typespec_name("int")), expr_name("void_ptr")),
        expr_compound(typespec_name("Vector"), list({ expr_int(1), expr_int(2) }), 2),
    };

    for (Expr** it = exprs; it != exprs + sizeof(exprs) / sizeof(*exprs); it++) {
        print_expr(*it);
        printf("\n");
    }

    printf("\n\n\n");

    // Statements
    Stmt *stmts[] = {
        stmt_return(expr_int(42)),
        stmt_break(),
        stmt_continue(),
        stmt_block(
            StmtBlock{
                list({
                    stmt_break(),
                    stmt_continue()
                }),
                2,
             }
        ),
        stmt_expr(expr_call(expr_name("print"), list({expr_int(1), expr_int(2)}), 2)),
        stmt_init("x", expr_int(42)),
        stmt_if(
            expr_name("flag1"),
            StmtBlock{
                list({
                    stmt_return(expr_int(1))
                }),
                1,
            },
            list_single({
                ElseIf{
                    expr_name("flag2"),
                    StmtBlock{
                        list({
                            stmt_return(expr_int(2))
                        }),
                        1,
                    }
                },
            }),
            1,
            StmtBlock{
                list({
                    stmt_return(expr_int(3))
                }),
                1,
            }
        ),
        stmt_while(
            expr_name("running"),
            StmtBlock{
                list({
                    stmt_assign(TokenKind::ADD_ASSIGN, expr_name("i"), expr_int(16)),
                }),
                1,
            }
        ),
        stmt_switch(
            expr_name("val"),
            list_single({
                SwitchCase{
                    list({expr_int(3), expr_int(4)}),
                    2,
                    false,
                    StmtBlock{
                        list({stmt_return(expr_name("val"))}),
                        1,
                    },
                },
                SwitchCase{
                    list({expr_int(1)}),
                    1,
                    true,
                    StmtBlock{
                        list({stmt_return(expr_int(0))}),
                        1,
                    },
                },
            }),
            2
        ),
    };
    for (Stmt **it = stmts; it != stmts + sizeof(stmts)/sizeof(*stmts); it++) {
        print_stmt(*it);
        printf("\n");
    }
}