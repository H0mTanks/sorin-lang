#include<types.hpp>
#include"Lex.hpp"

struct Expr;
struct Stmt;
struct Decl;
struct Typespec;

enum class TypespecKind {
    NONE,
    NAME,
    FUNC,
    ARRAY,
    PTR,
};

struct FuncTypespec {
    size_t num_args;
    Typespec** args;
    Typespec* ret;
};

struct ArrayTypespec {
    Typespec* elem;
    Expr* size;
};

struct PtrTypespec {
    Typespec* elem;
};

struct Typespec {
    TypespecKind kind;
    struct {//?not union?
        const char* name;
        FuncTypespec func;
        ArrayTypespec array;
        PtrTypespec ptr;
    };
};

Typespec* typespec_new(TypespecKind kind);

Typespec* typespec_name(const char* name);

Typespec* typespec_ptr(Typespec* elem);

Typespec* typespec_array(Typespec* elem, Expr* size);

Typespec* typespec_func(Typespec** args, size_t num_args, Typespec* ret);

enum class DeclKind {
    NONE,
    ENUM,
    STRUCT,
    UNION,
    VAR,
    CONST,
    TYPEDEF,
    FUNC,
};


enum class ExprKind {
    NONE,
    INT,
    FLOAT,
    STR,
    NAME,
    CAST,
    CALL,
    INDEX,
    FIELD,
    COMPOUND,
    UNARY,
    BINARY,
    TERNARY,
};

struct CompoundExpr {
    Typespec* type;
    Expr** args;
    size_t num_args;
};

struct CastExpr {
    Typespec* type;
    Expr* expr;
};

struct UnaryExpr {
    TokenKind op;
    Expr* expr;
};

struct BinaryExpr {
    TokenKind op;
    Expr* left;
    Expr* right;
};

struct TernaryExpr {
    Expr* cond;
    Expr* then_expr;
    Expr* else_expr;
};

struct CallExpr {
    Expr* expr;
    Expr** args;
    size_t num_args;
};

//?check field and index expr in dbg
struct IndexExpr {
    Expr* expr;
    Expr* index;
};

struct FieldExpr {
    Expr* expr;
    const char* name;
};

struct Expr {
    ExprKind kind;
    union {
        u64 int_val;
        f64 float_val;
        const char* str_val;
        const char* name;

        CompoundExpr compound;
        CastExpr cast;
        UnaryExpr unary;
        BinaryExpr binary;
        TernaryExpr ternary;
        CallExpr call;
        IndexExpr index;
        FieldExpr field;
    };
};

Expr* expr_new(ExprKind kind);

Expr* expr_int(u64 int_val);

Expr* expr_float(f64 float_val);

Expr* expr_str(const char* str_val);

Expr* expr_name(const char* name);

Expr* expr_compound(Typespec* type, Expr** args, size_t num_args);

Expr* expr_cast(Typespec* type, Expr* expr);

Expr* expr_call(Expr* expr, Expr** args, size_t num_args);

Expr* expr_index(Expr* expr, Expr* index);

Expr* expr_field(Expr* expr, const char* name);

Expr* expr_unary(TokenKind op, Expr* expr);

Expr* expr_binary(TokenKind op, Expr* left, Expr* right);

Expr* expr_ternary(Expr* cond, Expr* then_expr, Expr* else_expr);