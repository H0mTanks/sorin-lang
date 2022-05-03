#pragma once
#include <vector>
#include <types.hpp>

namespace Keywords {
    extern const char* typedef_keyword;
    extern const char* enum_keyword;
    extern const char* struct_keyword;
    extern const char* union_keyword;
    extern const char* var_keyword;
    extern const char* const_keyword;
    extern const char* func_keyword;
    extern const char* sizeof_keyword;
    extern const char* break_keyword;
    extern const char* continue_keyword;
    extern const char* return_keyword;
    extern const char* if_keyword;
    extern const char* else_keyword;
    extern const char* while_keyword;
    extern const char* do_keyword;
    extern const char* for_keyword;
    extern const char* switch_keyword;
    extern const char* case_keyword;
    extern const char* default_keyword;

    extern const char* first_keyword;
    extern const char* last_keyword;
};

enum class TokenKind {
    END_OF_FILE = 0,
    //*Reserve first 128 values for one-char tokens
    LAST_CHAR = 127,
    KEYWORD,
    INT,
    FLOAT,
    STR,
    NAME,
    LSHIFT,
    RSHIFT,
    EQ,
    NOTEQ,
    LTEQ,
    GTEQ,
    AND,
    OR,
    INC,
    DEC,
    COLON_ASSIGN,
    ADD_ASSIGN,
    FIRST_ASSIGN = ADD_ASSIGN, //?Should be colon assign
    SUB_ASSIGN,
    OR_ASSIGN,
    AND_ASSIGN,
    XOR_ASSIGN,
    LSHIFT_ASSIGN,
    RSHIFT_ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    LAST_ASSIGN = MOD_ASSIGN,
    SIZE_OF_ENUM,
};

enum class TokenMod {
    NONE,
    HEX,
    BIN,
    OCT,
    CHAR,
};

struct Token {
    TokenKind kind;
    TokenMod mod;
    const char* start;
    const char* end;
    union {
        u64 int_val;
        f64 float_val;
        const char* str_val;
        const char* name;
    };
};

void next_token();
void is_token();
void init_stream(const char* str);
void lex_test();