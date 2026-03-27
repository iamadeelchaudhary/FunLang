#include "env.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define ENV_BUCKETS 64

static uint32_t hash_string(const char* str) {
    uint32_t hash = 2166136261u;
    while (*str) {
        hash ^= (uint8_t)(*str++);
        hash *= 16777619u;
    }
    return hash;
}

Env* create_env(Env* parent) {
    Env* env = (Env*)malloc(sizeof(Env));
    env->parent = parent;
    env->num_buckets = ENV_BUCKETS;
    env->buckets = (EnvEntry**)calloc(ENV_BUCKETS, sizeof(EnvEntry*));
    return env;
}

void env_set(Env* env, const char* key, Value v) {
    uint32_t index = hash_string(key) % env->num_buckets;
    EnvEntry* entry = env->buckets[index];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            release_val(entry->value);
            entry->value = retain_val(v);
            return;
        }
        entry = entry->next;
    }
    
    EnvEntry* new_entry = (EnvEntry*)malloc(sizeof(EnvEntry));
    size_t klen = strlen(key);
    new_entry->key = (char*)malloc(klen + 1);
    strncpy(new_entry->key, key, klen + 1);
    new_entry->value = retain_val(v);
    new_entry->next = env->buckets[index];
    env->buckets[index] = new_entry;
}

bool env_get(Env* env, const char* key, Value* out_v) {
    Env* curr = env;
    while (curr) {
        uint32_t index = hash_string(key) % curr->num_buckets;
        EnvEntry* entry = curr->buckets[index];
        while (entry) {
            if (strcmp(entry->key, key) == 0) {
                *out_v = entry->value;
                return true;
            }
            entry = entry->next;
        }
        curr = curr->parent;
    }
    return false;
}

void free_env(Env* env) {
    if (!env) return;
    for (int i = 0; i < env->num_buckets; i++) {
        EnvEntry* entry = env->buckets[i];
        while (entry) {
            EnvEntry* next = entry->next;
            release_val(entry->value);
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    free(env->buckets);
    free(env);
}