#include "StringIntern.hpp"
#include "Globals.hpp"
#include "Lex.hpp"
#include <types.hpp>
#include <cassert>
#include <vector>

namespace Keywords {
    const char* typedef_keyword = nullptr;
    const char* enum_keyword = nullptr;
    const char* struct_keyword = nullptr;
    const char* union_keyword = nullptr;
    const char* var_keyword = nullptr;
    const char* const_keyword = nullptr;
    const char* func_keyword = nullptr;
    const char* sizeof_keyword = nullptr;
    const char* break_keyword = nullptr;
    const char* continue_keyword = nullptr;
    const char* return_keyword = nullptr;
    const char* if_keyword = nullptr;
    const char* else_keyword = nullptr;
    const char* while_keyword = nullptr;
    const char* do_keyword = nullptr;
    const char* for_keyword = nullptr;
    const char* switch_keyword = nullptr;
    const char* case_keyword = nullptr;
    const char* default_keyword = nullptr;

    const char* first_keyword = nullptr;
    const char* last_keyword = nullptr;

}

using namespace Keywords;

#define KEYWORD(name) name##_keyword = Global::string_table.add(#name); Global::keywords.push_back(name##_keyword)

Internal void init() {
    static bool inited;

    if (inited) {
        return;
    }

    char* arena_end = (char*)Global::string_table.arena.end;
    KEYWORD(typedef);
    KEYWORD(enum);
    KEYWORD(struct);
    KEYWORD(union);
    KEYWORD(const);
    KEYWORD(var);
    KEYWORD(func);
    KEYWORD(sizeof);
    KEYWORD(break);
    KEYWORD(continue);
    KEYWORD(return);
    KEYWORD(if);
    KEYWORD(else);
    KEYWORD(while);
    KEYWORD(do);
    KEYWORD(for);
    KEYWORD(switch);
    KEYWORD(case);
    KEYWORD(default);

    assert((char*)Global::string_table.arena.end == arena_end);

    first_keyword = typedef_keyword;
    last_keyword = default_keyword;
    inited = true;
}
#undef KEYWORD


Internal bool is_keyword_str(const char* str) {
    return first_keyword <= str && str <= last_keyword;
}


Internal void keywords_test() {
    init();
    assert(is_keyword_str(first_keyword));
    assert(is_keyword_str(last_keyword));

    for (const char* word : Global::keywords) {
        assert(is_keyword_str(word));
    }

    assert(!is_keyword_str(Global::string_table.add("foo")));
}

Internal void init_token_kind_names() {
    using namespace Global;
    token_kind_names.reserve((size_t)TokenKind::SIZE_OF_ENUM);

    for (int i = 0; i < (int)TokenKind::SIZE_OF_ENUM; i++) {
        token_kind_names.push_back("0");
    }

    token_kind_names[(int)TokenKind::END_OF_FILE] = "EOF";
    token_kind_names[(int)TokenKind::INT] = "int";
    token_kind_names[(int)TokenKind::FLOAT] = "float";
    token_kind_names[(int)TokenKind::STR] = "string";
    token_kind_names[(int)TokenKind::NAME] = "name";
    token_kind_names[(int)TokenKind::LSHIFT] = "<<";
    token_kind_names[(int)TokenKind::RSHIFT] = ">>";
    token_kind_names[(int)TokenKind::EQ] = "==";
    token_kind_names[(int)TokenKind::NOTEQ] = "!=";
    token_kind_names[(int)TokenKind::LTEQ] = "<=";
    token_kind_names[(int)TokenKind::GTEQ] = ">=";
    token_kind_names[(int)TokenKind::AND] = "&&";
    token_kind_names[(int)TokenKind::OR] = "||";
    token_kind_names[(int)TokenKind::INC] = "++";
    token_kind_names[(int)TokenKind::DEC] = "--";
    token_kind_names[(int)TokenKind::COLON_ASSIGN] = ":=";
    token_kind_names[(int)TokenKind::ADD_ASSIGN] = "+=";
    token_kind_names[(int)TokenKind::SUB_ASSIGN] = "-=";
    token_kind_names[(int)TokenKind::OR_ASSIGN] = "|=";
    token_kind_names[(int)TokenKind::AND_ASSIGN] = "&=";
    token_kind_names[(int)TokenKind::XOR_ASSIGN] = "^=";
    token_kind_names[(int)TokenKind::LSHIFT_ASSIGN] = "<<=";
    token_kind_names[(int)TokenKind::RSHIFT_ASSIGN] = ">>=";
    token_kind_names[(int)TokenKind::MUL_ASSIGN] = "*=";
    token_kind_names[(int)TokenKind::DIV_ASSIGN] = "/=";
    token_kind_names[(int)TokenKind::MOD_ASSIGN] = "%=";
}

Internal const char* token_kind_name(TokenKind kind) {
    if ((int)kind < Global::token_kind_names.size()) {
        return Global::token_kind_names[(int)kind];
    }
    else {
        return nullptr;
    }
}

void next_token() {
    
}

bool is_token(TokenKind kind) {
    return Global::token.kind == kind;
}


void init_stream(const char* str) {
    Global::stream = str;
    next_token();
}

void lex_test() {
    keywords_test();
    init_token_kind_names();
}