#ifndef FUNLANG_ENV_H
#define FUNLANG_ENV_H

#include "value.h"

typedef struct EnvEntry {
    char* key;
    Value value;
    struct EnvEntry* next;
} EnvEntry;

struct Env {
    EnvEntry** buckets;
    int num_buckets;
    struct Env* parent;
};

Env* create_env(Env* parent);
void env_set(Env* env, const char* key, Value v);
bool env_get(Env* env, const char* key, Value* out_v);
void free_env(Env* env);

#endif // FUNLANG_ENV_H