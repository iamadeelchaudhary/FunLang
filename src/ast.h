#ifndef FUNLANG_AST_H
#define FUNLANG_AST_H

#include <stddef.h>
#include "value.h"

typedef enum {
    AST_PROGRAM,
    AST_FUNC_CALL,
    AST_VAR,
    AST_ASSIGN,
    AST_LITERAL_STRING,
    AST_LITERAL_NUMBER,
    AST_LITERAL_BOOL,
    AST_LITERAL_NULL,
    AST_IDENTIFIER // Raw identifier (used for func names, args)
} ASTType;

struct AST {
    ASTType type;
    char* name;
    Value literal;
    struct AST** args;
    int arg_count;
    int line;
    int col;
    char* file;
};

// Arena allocator chunk
typedef struct ArenaChunk {
    char* data;
    size_t capacity;
    size_t used;
    struct ArenaChunk* next;
} ArenaChunk;

typedef struct {
    ArenaChunk* head;
} Arena;

// Arena functions
void arena_init(Arena* arena);
void* arena_alloc(Arena* arena, size_t size);
char* arena_strdup(Arena* arena, const char* str);
void arena_free(Arena* arena);

#endif // FUNLANG_AST_H