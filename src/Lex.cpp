#include "StringIntern.hpp"
#include "Globals.hpp"
#include "Lex.hpp"
#include <cctype>
#include <types.hpp>
#include <cassert>
#include <vector>
#include <sstream>

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


Internal bool is_keyword_name(const char* name) {
    return first_keyword <= name && name <= last_keyword;
}


Internal void keywords_test() {
    init();
    assert(is_keyword_name(first_keyword));
    assert(is_keyword_name(last_keyword));

    for (const char* word : Global::keywords) {
        assert(is_keyword_name(word));
    }

    assert(!is_keyword_name(Global::string_table.add("foo")));
}

Internal void init_token_kind_names() {
    using namespace Global;
    token_kind_names.reserve((size_t)TokenKind::SIZE_OF_ENUM);

    for (int i = 0; i < (int)TokenKind::SIZE_OF_ENUM; i++) {
        token_kind_names.push_back(nullptr);
    }

    token_kind_names[(int)TokenKind::END_OF_FILE] = "EOF";
    token_kind_names[(int)TokenKind::COLON] = ":",
    token_kind_names[(int)TokenKind::LPAREN] = "(",
    token_kind_names[(int)TokenKind::RPAREN] = ")",
    token_kind_names[(int)TokenKind::LBRACE] = "{",
    token_kind_names[(int)TokenKind::RBRACE] = "}",
    token_kind_names[(int)TokenKind::LBRACKET] = "[",
    token_kind_names[(int)TokenKind::RBRACKET] = "]",
    token_kind_names[(int)TokenKind::COMMA] = ",",
    token_kind_names[(int)TokenKind::DOT] = ".",
    token_kind_names[(int)TokenKind::QUESTION] = "?",
    token_kind_names[(int)TokenKind::SEMICOLON] = ";",
    token_kind_names[(int)TokenKind::KEYWORD] = "keyword",
    token_kind_names[(int)TokenKind::INT] = "int";
    token_kind_names[(int)TokenKind::FLOAT] = "float";
    token_kind_names[(int)TokenKind::STR] = "string";
    token_kind_names[(int)TokenKind::NAME] = "name";
    token_kind_names[(int)TokenKind::MUL] = "*",
    token_kind_names[(int)TokenKind::DIV] = "/",
    token_kind_names[(int)TokenKind::MOD] = "%",
    token_kind_names[(int)TokenKind::AND] = "&",
    token_kind_names[(int)TokenKind::LSHIFT] = "<<";
    token_kind_names[(int)TokenKind::RSHIFT] = ">>";
    token_kind_names[(int)TokenKind::ADD] = "+",
    token_kind_names[(int)TokenKind::SUB] = "-",
    token_kind_names[(int)TokenKind::OR] = "|",
    token_kind_names[(int)TokenKind::XOR] = "^",
    token_kind_names[(int)TokenKind::EQ] = "==";
    token_kind_names[(int)TokenKind::NOTEQ] = "!=";
    token_kind_names[(int)TokenKind::LT] = "<",
    token_kind_names[(int)TokenKind::GT] = ">",
    token_kind_names[(int)TokenKind::LTEQ] = "<=";
    token_kind_names[(int)TokenKind::GTEQ] = ">=";
    token_kind_names[(int)TokenKind::AND_AND] = "&&";
    token_kind_names[(int)TokenKind::OR_OR] = "||";
    token_kind_names[(int)TokenKind::ASSIGN] = "=",
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
    token_kind_names[(int)TokenKind::INC] = "++";
    token_kind_names[(int)TokenKind::DEC] = "--";
    token_kind_names[(int)TokenKind::COLON_ASSIGN] = ":=";
}

const char* token_kind_name(TokenKind kind) {
    if ((int)kind < Global::token_kind_names.size()) {
        return Global::token_kind_names[(int)kind];
    }
    else {
        return "<unknown>";
    }
}

const char* token_info()
{
    if (Global::token.kind == TokenKind::NAME || Global::token.kind == TokenKind::KEYWORD) {
        return Global::token.name;
    }
    else {
        return token_kind_name(Global::token.kind);
    }
}

Internal u8 hex_char_to_digit(u8 c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    
    if ((c >= 'a' && c <= 'f')) {
        return (c - 'a' + 10);
    }

    if (c >= 'A' && c <= 'F') {
        return (c - 'A' + 10);
    }

    return 0;
}


Internal void scan_int() {
    u64 base = 10;
    if (*Global::stream == '0') {
        Global::stream++;
        char c = *Global::stream;
        if (tolower(c) == 'x') {
            Global::stream++;
            Global::token.mod = TokenMod::HEX;
            base = 16;
        }
        else if (tolower(c) == 'b') {
            Global::stream++;
            Global::token.mod = TokenMod::BIN;
            base = 2;
        }
        else if (isdigit(*Global::stream)) {
            Global::token.mod = TokenMod::OCT;
            base = 8;
        }
    }

    u64 val = 0;
    while (true) {
        u8 c = *(u8*)Global::stream;
        u64 digit = hex_char_to_digit(c);
        
        if (digit == 0 && *Global::stream != '0') {
            //*char to digit returned an invalid value
            break;
        }

        if (digit >= base) {
            syntax_error("Digit '%c' out of range of base %llu", *Global::stream, base);
            digit = 0;
        }
        if (val > (UINT64_MAX - digit) / base) {
            syntax_error("Integer literal overflow");
            while (isdigit(*Global::stream)) {
                Global::stream++;
            }
            val = 0;
            break;
        }

        val = val * base + digit;
        Global::stream++;
    }

    Global::token.kind = TokenKind::INT;
    Global::token.int_val = val;
}

Internal void scan_float() {
    using namespace Global;
    const char* start = stream;

    //*take stream to the end of the number
    while(isdigit(*stream)) {
        stream++;
    }
    if (*stream == '.') {
        stream++;
    }
    while (isdigit(*stream)) {
        stream++;
    }

    if (tolower(*stream) == 'e') {
        stream++;
        if (*stream == '-' || *stream == '+') {
            stream++;
        }
        if (!isdigit(*stream)) {
            syntax_error("Expected digit after float literal exponent, found '%c'.", *stream);
        }
        while (isdigit(*stream)) {
            stream++;
        }
    }

    //*we now have a valid float in the stream
    f64 val = strtod(start,  nullptr);
    if (val == HUGE_VAL) {
        syntax_error("Float literal overflow");
    }
    token.kind = TokenKind::FLOAT;
    token.float_val = val;
}

Internal char char_to_escape(u8 c) {
    switch(c) {
        case 'n': return '\n';
        case 'r': return '\r';
        case 't': return '\t';
        case 'v': return '\v';
        case 'b': return '\b';
        case 'a': return '\a';
        case '0': return 0;
    }

    return 0;
}

Internal void scan_char() {
    using namespace Global;
    assert(*stream == '\'');
    stream++;

    char val = 0;
    if (*stream == '\'') {
        syntax_error("Char literal cannot be empty");
        stream++;
    }
    else if (*stream == '\n') {
        syntax_error("Char literal cannot contain newline");
    }
    else if (*stream == '\\') {
        stream++;
        val = char_to_escape(*(u8*)stream);
        if (val == 0 && *stream != '0') {
            syntax_error("Invalid char literal escape '\\%c'", *stream);
        }
        stream++;
    }
    else {
        val = *stream;
        stream++;
    }

    if (*stream != '\'') {
        syntax_error("Expected closing char quote, found '\\%c", *stream);
    }
    else {
        stream++;
    }

    token.kind = TokenKind::INT;
    token.int_val = val;
    token.mod = TokenMod::CHAR;
}

Internal void scan_str() {
    using namespace Global;
    assert(*stream == '"');
    stream++;

    std::ostringstream ss;
    while (*stream && *stream != '"') {
        char val = *stream;
        if (val == '\n') {
            syntax_error("String literal cannot contain newline");
            break;
        }
        else if (val == '\\') {
            stream++;
            val = char_to_escape(*(u8*)stream);
            if (val == 0 && *stream != '0') {
                syntax_error("Invalid string literal escape char '\\%c'", *stream);
            }
        }

        ss << val;
        stream++;
    }

    if (*stream) {
        assert(*stream == '"');
        stream++;
    }
    else {
        syntax_error("Unexpected end of file in string literal");
    }

    size_t sz = ss.str().size();
    char* str = (char*)malloc(sz + 1);
    strncpy(str, ss.str().c_str(), sz + 1);

    token.kind = TokenKind::STR;
    token.str_val = str;
}

Internal TokenKind op_single_kind(TokenKind kind) {
    Global::stream++;
    return kind;
}

Internal TokenKind op_double_kind(char op1, TokenKind kind1, char op2, TokenKind kind2) {
    TokenKind kind = kind1; // *Global::stream is op1
    Global::stream++;

    if (*Global::stream == op2) {
        kind = kind2;
        Global::stream++;
    }

    return kind;
}

Internal TokenKind op_triple_kind(char op1, TokenKind kind1, char op2, TokenKind kind2, char op3, TokenKind kind3) {
    TokenKind kind = kind1; // *Global::stream is op1
    Global::stream++;

    if (*Global::stream == op2) {
        kind = kind2;
        Global::stream++;
    }
    else if (*Global::stream == op3) {
        kind = kind3;
        Global::stream++;
    }

    return kind;
}

void next_token() {
    using namespace Global;
repeat:
    //get to start of each token
    while(isspace(*stream)) {
        stream++;
    }

    token.start = Global::stream;
    token.mod = TokenMod::NONE;

    switch(*stream) {
        case '\'': {
            scan_char();
            break;
        }
        case '"': {
            scan_str();
            break;
        }
        case '.': {
            if (isdigit(Global::stream[1])) {
                scan_float();
            }
            else {
                Global::token.kind = TokenKind::DOT;
                Global::stream++;
            }
            break;
        }
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
            while (isdigit(*stream)) {
                stream++;
            }

            char c = *stream;
            stream = token.start;

            if (c == '.' || c == 'e') {
                scan_float();
            }
            else {
                scan_int();
            }

            break;
        }
        //*keywords or identifiers
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case 'A': case 'B': case 'C': case 'D':
        case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
        case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z': case '_': {
            while (isalnum(*stream) || *stream == '_') {
                stream++;
            }
            token.name = string_table.add_range(token.start, stream);
            token.kind = is_keyword_name(token.name) ? TokenKind::KEYWORD : TokenKind::NAME;
            break;
        }
        //*operators
        case '<': {
            token.kind = TokenKind::LT;
            stream++;

            if (*stream == '<') {
                token.kind = TokenKind::LSHIFT;
                stream++;

                if (*stream == '=') {
                    token.kind = TokenKind::LSHIFT_ASSIGN;
                    stream++;
                }
            }
            else if (*stream == '=') {
                token.kind = TokenKind::LTEQ;
                stream++;
            }
            break;
        }
        case '>': {
            token.kind = TokenKind::GT;
            stream++;

            if (*stream == '>') {
                token.kind = TokenKind::RSHIFT;
                stream++;

                if (*stream == '=') {
                    token.kind = TokenKind::RSHIFT_ASSIGN;
                    stream++;
                }
            }
            else if (*stream == '=') {
                token.kind = TokenKind::GTEQ;
                stream++;
            }
            break;
        }
        case '\0': {
            token.kind = op_single_kind(TokenKind::END_OF_FILE);
            break;
        }
        case '(': {
            token.kind = op_single_kind(TokenKind::LPAREN);
            break;
        }
        case ')': {
            token.kind = op_single_kind(TokenKind::RPAREN);
            break;
        }
        case '{': {
            token.kind = op_single_kind(TokenKind::LBRACE);
            break;
        }
        case '}': {
            token.kind = op_single_kind(TokenKind::RBRACE);
            break;
        }
        case '[': {
            token.kind = op_single_kind(TokenKind::LBRACKET);
            break;
        }
        case ']': {
            token.kind = op_single_kind(TokenKind::RBRACKET);
            break;
        }
        case ',': {
            token.kind = op_single_kind(TokenKind::COMMA);
            break;
        }
        case '?': {
            token.kind = op_single_kind(TokenKind::QUESTION);
            break;
        }
        case ';': {
            token.kind = op_single_kind(TokenKind::SEMICOLON);
            break;
        }
        case ':': {
            token.kind = op_double_kind(':', TokenKind::COLON, '=', TokenKind::COLON_ASSIGN);
            break;
        }
        case '=': {
            token.kind = op_double_kind('=', TokenKind::ASSIGN, '=', TokenKind::EQ);
            break;
        }
        case '^': {
            token.kind = op_double_kind('^', TokenKind::XOR, '=', TokenKind::XOR_ASSIGN);
            break;
        }
        case '*': {
            token.kind = op_double_kind('*', TokenKind::MUL, '=', TokenKind::MUL_ASSIGN);
            break;
        }
        case '/': {
            token.kind = op_double_kind('/', TokenKind::DIV, '=', TokenKind::DIV_ASSIGN);
            break;
        }
        case '%': {
            token.kind = op_double_kind('%', TokenKind::MOD, '=', TokenKind::MOD_ASSIGN);
            break;
        }
        case '+': {
            token.kind = op_triple_kind('+', TokenKind::ADD, '=', TokenKind::ADD_ASSIGN, '+', TokenKind::INC);
            break;
        }
        case '-': {
            token.kind = op_triple_kind('-', TokenKind::SUB, '=', TokenKind::SUB_ASSIGN, '-', TokenKind::DEC);
            break;
        }
        case '&': {
            token.kind = op_triple_kind('&', TokenKind::AND, '=', TokenKind::AND_ASSIGN, '&', TokenKind::AND_AND);
            break;
        }
        case '|': {
            token.kind = op_triple_kind('|', TokenKind::OR, '=', TokenKind::OR_ASSIGN, '&', TokenKind::OR_OR);
            break;
        }
        default: {
            syntax_error("Invalid '%c' token character, skipping", *stream);
            stream++;
            goto repeat;
        }
    }

    Global::token.end = Global::stream;
}

bool is_token(TokenKind kind) {
    return Global::token.kind == kind;
}

bool is_token_eof()
{
    return Global::token.kind == TokenKind::END_OF_FILE;
}

bool is_token_name(const char* name)
{
    return Global::token.kind == TokenKind::NAME && Global::token.name == name;
}

bool is_keyword(const char* name)
{
    return is_token(TokenKind::KEYWORD) && Global::token.name == name;
}

bool match_keyword(const char* name)
{
    if (is_keyword(name)) {
        next_token();
        return true;
    }
    else {
        return false;
    }
}

bool match_token(TokenKind kind)
{
    if (is_token(kind)) {
        next_token();
        return true;
    }
    else {
        return false;
    }
}

bool expect_token(TokenKind kind)
{
    if (is_token(kind)) {
        next_token();
        return true;
    }
    else {
        fatal("Expected token: %s, got: %s", token_kind_name(kind), token_info());
        return false;
    }
}


void init_stream(const char* str) {
    Global::stream = str;
    next_token();
}

#define ASSERT_TOKEN(x) assert(match_token(static_cast<TokenKind>(x)))
#define ASSERT_TOKEN_NAME(x) assert(Global::token.name == Global::string_table.add(x) && match_token(TokenKind::NAME))
#define ASSERT_TOKEN_INT(x) assert(Global::token.int_val == (x) && match_token(TokenKind::INT))
#define ASSERT_TOKEN_FLOAT(x) assert(Global::token.float_val == (x) && match_token(TokenKind::FLOAT))
#define ASSERT_TOKEN_STR(x) assert(strcmp(Global::token.str_val, (x)) == 0 && match_token(TokenKind::STR))
#define ASSERT_TOKEN_EOF() assert(is_token_eof())


void lex_test() {
    keywords_test();
    init_token_kind_names();
    init_stream("0 18446744073709551615 0xffffffffffffffff 042 0b1111");

    //*integer literal tests
    ASSERT_TOKEN_INT(0);
    ASSERT_TOKEN_INT(18446744073709551615ull);
    assert(Global::token.mod == TokenMod::HEX);
    ASSERT_TOKEN_INT(0xffffffffffffffff);
    assert(Global::token.mod == TokenMod::OCT);
    ASSERT_TOKEN_INT(042);
    assert(Global::token.mod == TokenMod::BIN);
    ASSERT_TOKEN_INT(0xF);
    ASSERT_TOKEN_EOF();

    //*float literal tests
    init_stream("3.14 .123 42. 3e10");
    ASSERT_TOKEN_FLOAT(3.14);
    ASSERT_TOKEN_FLOAT(.123);
    ASSERT_TOKEN_FLOAT(42.);
    ASSERT_TOKEN_FLOAT(3e10);
    ASSERT_TOKEN_EOF();

    //*char literal tests
    init_stream("'a' '\\n'");
    ASSERT_TOKEN_INT('a');
    ASSERT_TOKEN_INT('\n');
    ASSERT_TOKEN_EOF();

    //* string literal tests
    init_stream("\"foo\" \"a\\nb\"");
    ASSERT_TOKEN_STR("foo");
    ASSERT_TOKEN_STR("a\nb");
    ASSERT_TOKEN_EOF();

    //*operator tests
    init_stream(": := + += ++ < <= << <<=");
    ASSERT_TOKEN(TokenKind::COLON);
    ASSERT_TOKEN(TokenKind::COLON_ASSIGN);
    ASSERT_TOKEN(TokenKind::ADD);
    ASSERT_TOKEN(TokenKind::ADD_ASSIGN);
    ASSERT_TOKEN(TokenKind::INC);
    ASSERT_TOKEN(TokenKind::LT);
    ASSERT_TOKEN(TokenKind::LTEQ);
    ASSERT_TOKEN(TokenKind::LSHIFT);
    ASSERT_TOKEN(TokenKind::LSHIFT_ASSIGN);
    ASSERT_TOKEN_EOF();

    //*misc tests
    init_stream("XY+(XY)_HELLO1,234+994");
    ASSERT_TOKEN_NAME("XY");
    ASSERT_TOKEN(TokenKind::ADD);
    ASSERT_TOKEN(TokenKind::LPAREN);
    ASSERT_TOKEN_NAME("XY");
    ASSERT_TOKEN(TokenKind::RPAREN);
    ASSERT_TOKEN_NAME("_HELLO1");
    ASSERT_TOKEN(TokenKind::COMMA);
    ASSERT_TOKEN_INT(234);
    ASSERT_TOKEN(TokenKind::ADD);
    ASSERT_TOKEN_INT(994);
    ASSERT_TOKEN_EOF();
}

#undef ASSERT_TOKEN
#undef ASSERT_TOKEN_NAME
#undef ASSERT_TOKEN_INT
#undef ASSERT_TOKEN_FLOAT
#undef ASSERT_TOKEN_STR
#undef ASSERT_TOKEN_EOF