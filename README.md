# sorin-lang
### Goals
- Easy to implement
- Immediately familiar to C programmers, no learning curve
- Reduce distractions, needless features
- Comfortable for day to day work, dogfooding
- Usable for host (Windows/Linux/Mac x86/x64) and target (RISC-V) development
- For host, interop seamlessly with C and host OS
- For target, generate machine code, don't need C compatibility
- Protect long-term code investment

### Non-goals
- "Best language ever" syndrome
- Highly opinionated vs C
- Raising level of abstraction
- Memory/integer overflow safety

### Why C?
- Appropriate level of abstraction
- Familiar to target audience
- Ecosystem, libraries, toolchain
- Code is protected investment

### But why not C?
- Needless compiler complexity, distracting gotchas, legacy cruft
- Challenging to build good non-compiler tooling
- Parsing requires symbol table, large/unbounded lookahead
- Type designator syntax too complex
- Implicit arithmetic conversions/integer promotions, notoriously bug prone
- Overuse of casts vs explicit conversion/construction
- No packages/modules, overreliance on preprocessor, physical coupling
- Undefined behavior everywhere

### Resolution
- Use the C type system, machine model, platform ABIs, etc
- Simple C-like syntax: LL(1), independent of symbol table
- Unabashedly low level, no GC, etc
- For target, emit machine code directly
- For host, emit isomorphic, idiomatic C code (real backend, not dumb string mapping)
- Easy to bootstrap, interop, portable (modulo ABI), toolchain for free
- Generates idiomatic C packages, protects code investment
- Unfortunately stuck with some undefined behavior from C :( unless...

### Quality of life
- Order-independent declarations, bonus from 2-pass
- Run-time introspection
- Fast, powerful non-compiler tooling, powered by compiler and introspection
- Packages, convention over configuration, relies on order-independent declarations

### Implementation
- Bootstrap as C99 codebase
- Eventually convert to self-hosting Sorin codebase to run on target
- C backend makes self-hosted bootstrapping from source easy: distribute generated C code
- Simple 2-pass compiler
- Pass 1, lex, LL(1) parse, produces AST
- Pass 2, resolve, type check, code gen, produces C code/RISC-V machine code/metadata

### Random cool stuff
- C backend to generate stb-style single file header-only libraries. Better C libs than C?

## WIP Grammar
### Tokens:

#### Grouping tokens:

( ) [ ] { }

#### Unary/binary operators

 \+ - ! ~ & *

LSHIFT = '<<'\
RSHIFT = '>>'\
EQ = '=='\
NOTEQ = '!='\
LTEQ = '<='\
GTEQ = '>='\
AND = '&&'\
OR = '||'

\+ - | ^ LSHIFT RSHIFT\
\* / % &\
EQ NOTEQ < LTEQ > GTEQ\
AND\
OR\
? :

#### Assignment operators:

COLON_ASSIGN = ':='\
ADD_ASSIGN = '+='\
SUB_ASSIGN = '-='\
OR_ASSIGN = '|='\
XOR_ASSIGN = '^='\
LSHIFT_ASSIGN = '<<='\
RSHIFT_ASSIGN = '>>='\
MUL_ASSIGN = '*='\
DIV_ASSIGN = '/='\
MOD_ASSIGN = '%='\
INC = '++'\
DEC = '--'


#### Names/literals:

NAME = [a-zA-Z_][a-zA-Z0-9_]*\
INT = 0 | [1-9][0-9]* | 0[xX][0-9a-fA-F]+ | 0[0-7]+ | 0[bB][0-1]+\
FLOAT = [0-9]*[.]?[0-9]*([eE][+-]?[0-9]+)?\
CHAR = '\'' . '\''\
STR = '"' [^"]* '"'

AST S-expression format:

func fact(n: int): int {\
    if (n == 0) {\
        return 1;\
    } else {\
        return n * fact(n-1);\
    }\
}
\
(func fact (n int) int\
  (if (== n 0)\
    (then\
      (return 1))\
    (else\
      (return (* n (fact (- n 1)))))))

### EBNF grammar:

#### Declarations:

type_list = type (',' type)*\
name_list = NAME (',' NAME)*\
\
typedef f = func(int):int\
typedef f = func(int):int[16]\
typedef f = (func(int):int)[16]\
\
base_type = NAME\
          | 'func' '(' type_list? ')' (':' type)?\
          | '(' type ')'\
type = base_type ('[' expr? ']' | '*')*\
\
enum_item = NAME ('=' expr)?\
enum_items = enum_item (',' enum_item)* ','?\
enum_decl = NAME '{' enum_items? '}'\
\
aggregate_field = name_list ':' type ';'\
aggregate_decl = NAME '{' aggregate_field* '}'\
\
var_decl = NAME '=' expr\
         | NAME ':' type ('=' expr)?\

const_decl = NAME '=' expr\

typedef_decl = NAME '=' type\

func_param = NAME ':' type\
func_param_list = func_param (',' func_param)*\
func_decl = NAME '(' func_param_list? ')' (':' type)? stmt_block\

decl = 'enum' enum_decl\
     | 'struct' aggregate_decl\
     | 'union' aggregate_decl\
     | 'var' var_decl\
     | 'const' const_decl\
     | 'typedef' typedef_decl\
     | 'func' func_decl\

### Statements:
\
assign_op = '=' | COLON_ASSIGN | ADD_ASSIGN | ...\
\
switch_case = (CASE expr | DEFAULT) ':' stmt*\
switch_block = '{' switch_case* '}'\

stmt = 'return' expr ';'\
     | 'break' ';'\
     | 'continue' ';'\
     | '{' stmt* '}'\
     | 'if' '(' expr ')' stmt_block ('else' 'if' '(' expr ')' stmt_block)* ('else' stmt_block)?\
     | 'while' '(' expr ')' stmt_block\
     | 'for' '(' stmt_list ';' expr ';' stmt_list ')' stmt_block\
     | 'do' stmt_block 'while' '(' expr ')' ';'\
     | switch '(' expr ')' switch_block\
     | expr (INC | DEC | assign_op expr)?\
\
int i;\
sizeof(i)\
sizeof(int)\
\
x := 1\
y := 2\
n := sizeof(x + y)\
m := sizeof(:int[16]) // 16*sizeof(int)\
\
var v: Vector\
\
v := Vector{1,2}\
v := (:Vector){1,2} // synonym for prev line\
a := (:Vector*[3]){p,q,r}\
\
u8 := uint8(42)\
u32 = uint32(u8)\
\
v := Vector{1,2,3}\
v := (:Vector){1,2,3}\
vs := (:Vector[]){1,2,3}\
\
u := uintptr(0x1234678)\
ptr := (:int*)u\
\
typespec = NAME | '(' ':' type ')'\
\
operand_expr = INT\
             | FLOAT\
             | STR\
             | NAME\
             | typespec? '{' expr_list '}'\
             | CAST '(' type ',' expr ')'\
             | '(' expr ')'\
base_expr = operand_expr ('(' expr_list ')' | '[' expr ']' | '.' NAME)*\
unary_expr = [+-~!&*] unary_expr\
           | base_expr\
mul_op = [*/%&] | LSHIFT | RSHIFT\
mul_expr = unary_expr (mul_op unary_expr)*\
add_op = [+-|^]\
add_expr = mul_expr (add_op mul_expr)*\
cmp_op = [<>] | EQ | NOTEQ | LTEQ | GTEQ\
cmp_expr = add_expr (cmp_op add_expr)*\
and_expr = cmp_expr (AND cmp_expr)*\
or_expr = and_expr (OR and_expr)*\
ternary_expr = or_expr ('?' ternary_expr ':' ternary_expr)?\
expr = ternary_expr\
\
