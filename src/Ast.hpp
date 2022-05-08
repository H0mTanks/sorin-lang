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

enum class StmtKind {
    NONE,
    RETURN,
    BREAK,
    CONTINUE,
    BLOCK,
    IF,
    WHILE,
    DO_WHILE,
    FOR,
    DO,
    SWITCH,
    ASSIGN,
    INIT,
    EXPR,
};

struct StmtBlock {
    Stmt** stmts;
    size_t num_stmts;
};

struct ReturnStmt {
    Expr* expr;
};

struct ElseIf {
    Expr* cond;
    StmtBlock block;
};

struct IfStmt {
    Expr* cond;
    StmtBlock then_block;
    ElseIf* elseifs;
    size_t num_elseifs;
    StmtBlock else_block;
};

struct WhileStmt {
    Expr* cond;
    StmtBlock block;
};

struct ForStmt {
    Stmt* init;
    Expr* cond;
    Stmt* next;
    StmtBlock block;
};

struct SwitchCase {
    Expr** exprs;
    size_t num_exprs;
    bool is_default;
    StmtBlock block;
};

struct SwitchStmt {
    Expr* expr;
    SwitchCase* cases;
    size_t num_cases;
};

struct AssignStmt {
    TokenKind op;
    Expr* left;
    Expr* right;
};

struct InitStmt {
    const char* name;
    Expr* expr;
};

struct Stmt {
    StmtKind kind;
    union {
        ReturnStmt return_stmt;
        IfStmt if_stmt;
        WhileStmt while_stmt;
        ForStmt for_stmt;
        SwitchStmt switch_stmt;
        StmtBlock block;
        AssignStmt assign;
        InitStmt init;
        Expr* expr;
    };
};

Stmt* stmt_return(Expr* expr);

Stmt* stmt_break();

Stmt* stmt_continue();

Stmt* stmt_block(StmtBlock block);

Stmt* stmt_if(Expr* cond, StmtBlock then_block, ElseIf* elseifs, size_t num_elseifs, StmtBlock else_block);

Stmt* stmt_while(Expr* cond, StmtBlock block);

Stmt* stmt_do_while(Expr* cond, StmtBlock block);

Stmt* stmt_for(Stmt* init, Expr* cond, Stmt* next, StmtBlock block);

Stmt* stmt_switch(Expr* expr, SwitchCase* cases, size_t num_cases);

Stmt* stmt_assign(TokenKind op, Expr* left, Expr* right);

Stmt* stmt_init(const char* name, Expr* expr);

Stmt* stmt_expr(Expr* expr);