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