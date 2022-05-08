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

template<typename T>
T** list(const std::initializer_list<T*>& lst) {
    return const_cast<T**>(lst.begin());
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
}