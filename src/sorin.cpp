#include <iostream>
#include <types.hpp>
#include "StringIntern.hpp"
#include "Globals.hpp"

#define ALIGN_DOWN(n, a) ((n) & ~((a) - 1))
#define ALIGN_UP(n, a) ALIGN_DOWN((n) + (a) - 1, (a))
#define MAX(a,b) (((a) >= (b)) ? (a) : (b))
#define ARENA_ALIGNMENT 8
#define ARENA_BLOCK_SIZE 1024

int main() {
    std::cout << "hello world\n";
    string_table.intern_test();

    for (Intern const& intern : string_table.interns) {
        std::cout << intern.str << std::endl;
    }
}