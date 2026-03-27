#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include "value.h"
#include <stdlib.h>
#include <string.h>

static char* safe_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char* dup = (char*)malloc(len + 1);
    if (dup) {
        strncpy(dup, s, len);
        dup[len] = '\0';
    }
    return dup;
}

Value create_number(double n) {
    Value v; v.type = TYPE_NUMBER; v.as.number = n; return v;
}

Value create_bool(bool b) {
    Value v; v.type = TYPE_BOOL; v.as.boolean = b; return v;
}

Value create_null(void) {
    Value v; v.type = TYPE_NULL; return v;
}

Value create_string(const char* s) {
    Value v; v.type = TYPE_STRING;
    v.as.string = (StringVal*)malloc(sizeof(StringVal));
    v.as.string->str = safe_strdup(s ? s : "");
    v.as.string->ref_count = 1;
    return v;
}

Value create_list(void) {
    Value v; v.type = TYPE_LIST;
    v.as.list = (ListVal*)malloc(sizeof(ListVal));
    v.as.list->capacity = 4;
    v.as.list->count = 0;
    v.as.list->items = (Value*)malloc(sizeof(Value) * 4);
    v.as.list->ref_count = 1;
    return v;
}

Value create_func(AST* body, AST* args_node, Env* env) {
    Value v; v.type = TYPE_FUNCTION;
    v.as.func = (FuncVal*)malloc(sizeof(FuncVal));
    v.as.func->body = body;
    v.as.func->args_node = args_node;
    v.as.func->captured_env = env; // Not ref-counted to prevent cycles
    v.as.func->ref_count = 1;
    return v;
}

Value create_builtin(BuiltinFunc func, const char* name) {
    Value v; v.type = TYPE_BUILTIN;
    v.as.builtin.func = func;
    v.as.builtin.name = name;
    return v;
}

Value create_file(FILE* fp) {
    Value v; v.type = TYPE_FILE;
    v.as.file = (FileVal*)malloc(sizeof(FileVal));
    v.as.file->fp = fp;
    v.as.file->is_open = (fp != NULL);
    v.as.file->ref_count = 1;
    return v;
}

Value retain_val(Value v) {
    if (v.type == TYPE_STRING) v.as.string->ref_count++;
    else if (v.type == TYPE_LIST) v.as.list->ref_count++;
    else if (v.type == TYPE_FUNCTION) v.as.func->ref_count++;
    else if (v.type == TYPE_FILE) v.as.file->ref_count++;
    return v;
}

void release_val(Value v) {
    if (v.type == TYPE_STRING) {
        v.as.string->ref_count--;
        if (v.as.string->ref_count <= 0) {
            free(v.as.string->str);
            free(v.as.string);
        }
    } else if (v.type == TYPE_LIST) {
        v.as.list->ref_count--;
        if (v.as.list->ref_count <= 0) {
            for (int i = 0; i < v.as.list->count; i++) {
                release_val(v.as.list->items[i]);
            }
            free(v.as.list->items);
            free(v.as.list);
        }
    } else if (v.type == TYPE_FUNCTION) {
        v.as.func->ref_count--;
        if (v.as.func->ref_count <= 0) {
            free(v.as.func);
        }
    } else if (v.type == TYPE_FILE) {
        v.as.file->ref_count--;
        if (v.as.file->ref_count <= 0) {
            if (v.as.file->is_open && v.as.file->fp) {
                fclose(v.as.file->fp);
            }
            free(v.as.file);
        }
    }
}

void list_push(Value list_val, Value item) {
    if (list_val.type != TYPE_LIST) return;
    ListVal* l = list_val.as.list;
    if (l->count >= l->capacity) {
        l->capacity *= 2;
        l->items = (Value*)realloc(l->items, sizeof(Value) * l->capacity);
    }
    l->items[l->count++] = retain_val(item);
}