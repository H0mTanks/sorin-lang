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
    END_OF_FILE,
    COLON,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    COMMA,
    DOT,
    QUESTION,
    SEMICOLON,
    KEYWORD,
    INT,
    FLOAT,
    STR,
    NAME,
    //*multiplicative precedence
    FIRST_MUL,
    MUL = FIRST_MUL,
    DIV,
    MOD,
    AND,
    LSHIFT,
    RSHIFT,
    LAST_MUL = RSHIFT,
    //*additive precedence
    FIRST_ADD,
    ADD = FIRST_ADD,
    SUB,
    XOR,
    OR,
    LAST_ADD = OR,
    //*comparative precedence
    FIRST_CMP,
    EQ = FIRST_CMP,
    NOTEQ,
    LT,
    GT,
    LTEQ,
    GTEQ,
    LAST_CMP = GTEQ,
    AND_AND,
    OR_OR,
    //*Assignment operators
    FIRST_ASSIGN,
    ASSIGN = FIRST_ASSIGN,
    ADD_ASSIGN,
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
    INC,
    DEC,
    COLON_ASSIGN,
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
bool is_token(TokenKind kind);
bool is_token_eof();
bool is_token_name(const char* name);
bool is_keyword(const char* name);
bool match_keyword(const char* name);
bool match_token(TokenKind kind);
bool expect_token(TokenKind kind);
const char* token_info();
const char* token_kind_name(TokenKind kind);

void init_stream(const char* str);
void lex_test();