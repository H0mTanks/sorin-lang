#include <iostream>
#include <types.hpp>
#include "StringIntern.hpp"
#include "Globals.hpp"
#include "Lex.hpp"
#include "Print.hpp"
#include "Parse.hpp"
#include <sstream>

#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))
#define MAX(a,b) (((a) >= (b)) ? (a) : (b))
#define ARENA_ALIGNMENT 8
#define ARENA_BLOCK_SIZE 1024

int main() {
    std::cout << "Running main\n";
    Global::string_table.intern_test();

    lex_test();

    for (Intern const& intern : Global::string_table.interns) {
        std::cout << intern.str << std::endl;
    }

    std::cout << "TokenKind names" << std::endl;
    for (int i = 0; i < (int)TokenKind::SIZE_OF_ENUM; i++) {
        if (!Global::token_kind_names[i]) {
            continue;
        }

        std::cout << Global::token_kind_names[i] << " " << i << std::endl;
    }

    print_tests();

    printf("\n\n\n");

    parse_test();

}