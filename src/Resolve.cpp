#include <cassert>
#include "Resolve.hpp"
#include "Globals.hpp"
#include "StringIntern.hpp"

Sym* sym_get(const char* name) {
    for (Sym& it : Global::syms) {
        if (it.name == name) {
            return &it;
        }
    }

    return nullptr;
}

void sym_put(Decl* decl) {
    assert(decl->name);
    assert(!sym_get(decl->name));
    Global::syms.push_back(Sym{decl->name, decl, SymState::UNRESOLVED});
}

void resolve_decl(Decl* decl) {
    switch (decl->kind) {
        case DeclKind::CONST: {
            //ConstEntity const_ent = resolve_const_expr(decl->const_decl.expr);
            
            break;
        }
    }
}

void resolve_sym(Sym* sym) {
    if (sym->state == SymState::RESOLVED) {
        return;
    }
    if (sym->state == SymState::RESOLVING) {
        fatal("Cyclic dependency");
        return;
    }
    resolve_decl(sym->decl);
}

Sym* resolve_name(const char* name) {
    Sym* sym = sym_get(name);
    if (!sym) {
        fatal("Unknown name");
        return nullptr;
    }

    resolve_sym(sym);
    return sym;
}

void resolve_syms() {
    for (Sym& it : Global::syms) {
        resolve_sym(&it);
    }
}

Type* type_alloc(TypeKind kind) {
    Type* t = (Type*)xcalloc(1, sizeof(Type));
    t->kind = kind;
    return t;
}

Type* type_ptr(Type* base) {
    for (CachedPtrType& it : Global::cached_ptr_types) {
        if (it.base == base) {
            return it.ptr;
        }
    }

    Type* t = type_alloc(TypeKind::PTR);
    t->ptr.base = base;
    Global::cached_ptr_types.push_back({base, t});
    return t;
}

Type* type_array(Type* base, size_t size) {
    for (CachedArrayType& it : Global::cached_array_types) {
        if (it.base == base && it.size == size) {
            return it.array;
        }
    }

    Type* t = type_alloc(TypeKind::ARRAY);
    t->array.base = base;
    t->array.size = size;
    Global::cached_array_types.push_back({base, size, t});
    return t;
}

Type* type_func(Type** params, size_t num_params, Type* ret) {
    for (CachedFuncType& it : Global::cached_func_types) {
        if (it.num_params == num_params && it.ret == ret) {
            bool match = true;
            for (size_t i = 0; i < num_params; i++) {
                if (it.params[i] != params[i]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                return it.func;
            }
        }
    }

    Type* t = type_alloc(TypeKind::FUNC);
    t->func.params = (Type**)xcalloc(num_params, sizeof(Type*));
    memcpy(t->func.params, params, num_params * sizeof(Type*));
    t->func.num_params = num_params;
    t->func.ret = ret;

    Global::cached_func_types.push_back({params, num_params, ret, t});
    return t;
}

Type* type_struct(TypeField* fields, size_t num_fields) {
    Type* t = type_alloc(TypeKind::STRUCT);
    t->aggregate.fields = (TypeField*)xcalloc(num_fields, sizeof(TypeField));
    memcpy(t->aggregate.fields, fields, num_fields * sizeof(TypeField));
    t->aggregate.num_fields = num_fields;

    return t;
}

Type* type_union(TypeField* fields, size_t num_fields) {
    Type* t = type_alloc(TypeKind::UNION);
    t->aggregate.fields = (TypeField*)xcalloc(num_fields, sizeof(TypeField));
    memcpy(t->aggregate.fields, fields, num_fields * sizeof(TypeField));
    t->aggregate.num_fields = num_fields;

    return t;
}

void resolve_test() {
    using Global::type_int;
    using Global::type_float;

    const char* foo = Global::string_table.add("foo");
    assert(sym_get(foo) == nullptr);
    Decl* decl = decl_const(foo, expr_int(42));
    sym_put(decl);
    Sym* sym = sym_get(foo);
    assert(sym && sym->decl == decl);

    Type* int_ptr = type_ptr(type_int);
    assert(type_ptr(type_int) == int_ptr);
    Type* float_ptr = type_ptr(type_float);
    assert(type_ptr(type_float) == float_ptr);
    assert(int_ptr != float_ptr);
    Type* int_ptr_ptr = type_ptr(type_ptr(type_int));
    assert(type_ptr(type_ptr(type_int)) == int_ptr_ptr);
    Type* float4_array = type_array(type_float, 4);
    assert(type_array(type_float, 4) == float4_array);
    Type* float3_array = type_array(type_float, 3);
    assert(type_array(type_float, 3) == float3_array);
    assert(float4_array != float3_array);
    Type* int_int_func = type_func(&type_int, 1, type_int);
    assert(type_func(&type_int, 1, type_int) == int_int_func);
    Type* int_func = type_func(NULL, 0, type_int);
    assert(int_int_func != int_func);
    assert(int_func == type_func(NULL, 0, type_int));
}
