#include <iostream>
#include <types.hpp>
#include "StringIntern.hpp"
#include "Globals.hpp"

int main() {
    std::cout << "hello world\n";
    string_table.intern_test();

    for (Intern const& intern : string_table.interns) {
        std::cout << intern.str << std::endl;
    }
}