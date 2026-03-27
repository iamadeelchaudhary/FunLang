#ifndef FUNLANG_VALUE_H
#define FUNLANG_VALUE_H

#include <stdbool.h>
#include <stdio.h>

// Forward declarations
typedef struct AST AST;
typedef struct Env Env;
typedef struct EvalContext EvalContext;

typedef enum {
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_NULL,
    TYPE_LIST,
    TYPE_FUNCTION,
    TYPE_BUILTIN,
    TYPE_FILE
} ValueType;

typedef struct Value Value;

typedef struct {
    char* str;
    int ref_count;
} StringVal;

typedef struct {
    Value* items;
    int count;
    int capacity;
    int ref_count;
} ListVal;

typedef struct {
    AST* body;
    AST* args_node;
    Env* captured_env;
    int ref_count;
} FuncVal;

typedef Value (*BuiltinFunc)(Value* args, int arg_count, EvalContext* ctx);

typedef struct {
    BuiltinFunc func;
    const char* name;
} BuiltinVal;

typedef struct {
    FILE* fp;
    bool is_open;
    int ref_count;
} FileVal;

struct Value {
    ValueType type;
    union {
        double number;
        bool boolean;
        StringVal* string;
        ListVal* list;
        FuncVal* func;
        BuiltinVal builtin;
        FileVal* file;
    } as;
};

// Value creation
Value create_number(double n);
Value create_bool(bool b);
Value create_null(void);
Value create_string(const char* s);
Value create_list(void);
Value create_func(AST* body, AST* args_node, Env* env);
Value create_builtin(BuiltinFunc func, const char* name);
Value create_file(FILE* fp);

// Memory management (Reference Counting)
Value retain_val(Value v);
void release_val(Value v);
void list_push(Value list_val, Value item);

#endif // FUNLANG_VALUE_H