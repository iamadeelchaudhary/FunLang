#include "ast.h"
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 65536

void arena_init(Arena* arena) {
    arena->head = NULL;
}

void* arena_alloc(Arena* arena, size_t size) {
    // Alignment requirement
    size = (size + 7) & ~7;
    
    if (!arena->head || arena->head->used + size > arena->head->capacity) {
        size_t cap = size > CHUNK_SIZE ? size : CHUNK_SIZE;
        ArenaChunk* chunk = (ArenaChunk*)malloc(sizeof(ArenaChunk));
        chunk->data = (char*)malloc(cap);
        chunk->capacity = cap;
        chunk->used = 0;
        chunk->next = arena->head;
        arena->head = chunk;
    }
    
    void* ptr = arena->head->data + arena->head->used;
    arena->head->used += size;
    return ptr;
}

char* arena_strdup(Arena* arena, const char* str) {
    if (!str) return NULL;
    size_t len = strlen(str) + 1;
    char* dup = (char*)arena_alloc(arena, len);
    memcpy(dup, str, len);
    return dup;
}

void arena_free(Arena* arena) {
    ArenaChunk* curr = arena->head;
    while (curr) {
        ArenaChunk* next = curr->next;
        free(curr->data);
        free(curr);
        curr = next;
    }
    arena->head = NULL;
}