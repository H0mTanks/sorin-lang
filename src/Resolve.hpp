#pragma once
#include "Ast.hpp"
#include <types.hpp>

// enum class EntityKind {

// };

enum class TypeKind {
    INT,
    FLOAT,
    PTR,
    ARRAY,
    STRUCT,
    UNION,
    FUNC,
};

struct Type;

struct TypeField {
    const char* name;
    Type* type;
};

struct Type {
    TypeKind kind;
    union {
        struct {
            Type* base;
        } ptr;
        struct {
            Type* base;
            size_t size;
        } array;
        struct {
            TypeField* fields;
            size_t num_fields;
        } aggregate;
        struct {
            Type** params;
            size_t num_params;
            Type* ret;
        } func;
    };
};

Type* type_alloc(TypeKind kind);

struct CachedPtrType {
    Type* base;
    Type* ptr;
};

Type* type_ptr(Type* base);

struct CachedArrayType {
    Type* base;
    size_t size;
    Type* array;
};

Type* type_array(Type* base, size_t size);

struct CachedFuncType {
    Type** params;
    size_t num_params;
    Type* ret;
    Type* func;
};

Type* type_func(Type** params, size_t num_params, Type* ret);

Type* type_struct(TypeField* fields, size_t num_fields);
Type* type_union(TypeField* fields, size_t num_fields);

struct ConstEntity {
    Type* type;
    union {
        u64 int_val;
        double float_val;
    };
};

struct Entity {
    int foo;
};

enum class SymState {
    UNRESOLVED,
    RESOLVING,
    RESOLVED,
};

struct Sym {
    const char* name;
    Decl* decl;
    SymState state;
    Entity* ent;
};

Sym* sym_get(const char* name);

void sym_put(Decl* decl);

void resolve_syms();

void resolve_test();