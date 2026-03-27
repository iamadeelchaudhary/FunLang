#include "builtins.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static Value builtin_print(Value* args, int count, EvalContext* ctx) {
    if (count != 1) { set_error(ctx, "ArgError", "print expects 1 argument"); return create_null(); }
    if (args[0].type == TYPE_STRING) printf("%s", args[0].as.string->str);
    else if (args[0].type == TYPE_NUMBER) printf("%g", args[0].as.number);
    else if (args[0].type == TYPE_BOOL) printf("%s", args[0].as.boolean ? "true" : "false");
    else if (args[0].type == TYPE_NULL) printf("null");
    else printf("<object>");
    return create_null();
}

static Value builtin_println(Value* args, int count, EvalContext* ctx) {
    builtin_print(args, count, ctx);
    if (!ctx->has_error) printf("\n");
    return create_null();
}

static Value builtin_add(Value* args, int count, EvalContext* ctx) {
    if (count != 2) { set_error(ctx, "ArgError", "add expects 2 arguments"); return create_null(); }
    if (args[0].type == TYPE_NUMBER && args[1].type == TYPE_NUMBER) {
        return create_number(args[0].as.number + args[1].as.number);
    }
    if (args[0].type == TYPE_STRING && args[1].type == TYPE_STRING) {
        size_t l1 = strlen(args[0].as.string->str);
        size_t l2 = strlen(args[1].as.string->str);
        char* merged = malloc(l1 + l2 + 1);
        strcpy(merged, args[0].as.string->str);
        strcat(merged, args[1].as.string->str);
        Value v = create_string(merged);
        free(merged);
        return v;
    }
    set_error(ctx, "TypeError", "add requires two numbers or two strings");
    return create_null();
}

static Value builtin_sub(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_NUMBER || args[1].type != TYPE_NUMBER) {
        set_error(ctx, "TypeError", "sub requires two numbers"); return create_null();
    }
    return create_number(args[0].as.number - args[1].as.number);
}

static Value builtin_mul(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_NUMBER || args[1].type != TYPE_NUMBER) {
        set_error(ctx, "TypeError", "mul requires two numbers"); return create_null();
    }
    return create_number(args[0].as.number * args[1].as.number);
}

static Value builtin_div(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_NUMBER || args[1].type != TYPE_NUMBER) {
        set_error(ctx, "TypeError", "div requires two numbers"); return create_null();
    }
    if (args[1].as.number == 0) {
        set_error(ctx, "MathError", "Division by zero"); return create_null();
    }
    return create_number(args[0].as.number / args[1].as.number);
}

static Value builtin_mod(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_NUMBER || args[1].type != TYPE_NUMBER) {
        set_error(ctx, "TypeError", "mod requires two numbers"); return create_null();
    }
    if (args[1].as.number == 0) {
        set_error(ctx, "MathError", "Modulo by zero"); return create_null();
    }
    return create_number(fmod(args[0].as.number, args[1].as.number));
}

static Value builtin_pow(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_NUMBER || args[1].type != TYPE_NUMBER) {
        set_error(ctx, "TypeError", "pow requires two numbers"); return create_null();
    }
    return create_number(pow(args[0].as.number, args[1].as.number));
}

static Value builtin_eq(Value* args, int count, EvalContext* ctx) {
    if (count != 2) { set_error(ctx, "ArgError", "eq expects 2 arguments"); return create_null(); }
    if (args[0].type != args[1].type) return create_bool(false);
    if (args[0].type == TYPE_NUMBER) return create_bool(args[0].as.number == args[1].as.number);
    if (args[0].type == TYPE_STRING) return create_bool(strcmp(args[0].as.string->str, args[1].as.string->str) == 0);
    if (args[0].type == TYPE_BOOL) return create_bool(args[0].as.boolean == args[1].as.boolean);
    if (args[0].type == TYPE_NULL) return create_bool(true);
    return create_bool(false);
}

static Value builtin_neq(Value* args, int count, EvalContext* ctx) {
    Value eq_res = builtin_eq(args, count, ctx);
    if (ctx->has_error) return create_null();
    return create_bool(!eq_res.as.boolean);
}

static Value builtin_gt(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_NUMBER || args[1].type != TYPE_NUMBER) {
        set_error(ctx, "TypeError", "gt requires two numbers"); return create_null();
    }
    return create_bool(args[0].as.number > args[1].as.number);
}

static Value builtin_lt(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_NUMBER || args[1].type != TYPE_NUMBER) {
        set_error(ctx, "TypeError", "lt requires two numbers"); return create_null();
    }
    return create_bool(args[0].as.number < args[1].as.number);
}

static Value builtin_gte(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_NUMBER || args[1].type != TYPE_NUMBER) {
        set_error(ctx, "TypeError", "gte requires two numbers"); return create_null();
    }
    return create_bool(args[0].as.number >= args[1].as.number);
}

static Value builtin_lte(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_NUMBER || args[1].type != TYPE_NUMBER) {
        set_error(ctx, "TypeError", "lte requires two numbers"); return create_null();
    }
    return create_bool(args[0].as.number <= args[1].as.number);
}

static Value builtin_and(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_BOOL || args[1].type != TYPE_BOOL) {
        set_error(ctx, "TypeError", "and requires two booleans"); return create_null();
    }
    return create_bool(args[0].as.boolean && args[1].as.boolean);
}

static Value builtin_or(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_BOOL || args[1].type != TYPE_BOOL) {
        set_error(ctx, "TypeError", "or requires two booleans"); return create_null();
    }
    return create_bool(args[0].as.boolean || args[1].as.boolean);
}

static Value builtin_not(Value* args, int count, EvalContext* ctx) {
    if (count != 1 || args[0].type != TYPE_BOOL) {
        set_error(ctx, "TypeError", "not requires a boolean"); return create_null();
    }
    return create_bool(!args[0].as.boolean);
}

static Value builtin_give_back(Value* args, int count, EvalContext* ctx) {
    if (count != 1) { set_error(ctx, "ArgError", "give_back expects 1 argument"); return create_null(); }
    ctx->returned = true;
    ctx->return_value = retain_val(args[0]);
    return create_null();
}

static Value builtin_list(Value* args, int count, EvalContext* ctx) {
    (void)ctx;
    Value l = create_list();
    for (int i = 0; i < count; i++) list_push(l, args[i]);
    return l;
}

static Value builtin_item(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_LIST || args[1].type != TYPE_NUMBER) {
        set_error(ctx, "TypeError", "item expects list and index"); return create_null();
    }
    int idx = (int)args[1].as.number;
    ListVal* l = args[0].as.list;
    if (idx < 0 || idx >= l->count) {
        set_error(ctx, "IndexError", "Index %d out of bounds", idx); return create_null();
    }
    return retain_val(l->items[idx]);
}

static Value builtin_push(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_LIST) {
        set_error(ctx, "TypeError", "push expects list and item"); return create_null();
    }
    list_push(args[0], args[1]);
    return create_null();
}

static Value builtin_list_len(Value* args, int count, EvalContext* ctx) {
    if (count != 1 || args[0].type != TYPE_LIST) {
        set_error(ctx, "TypeError", "list_len expects a list"); return create_null();
    }
    return create_number(args[0].as.list->count);
}

static Value builtin_explode(Value* args, int count, EvalContext* ctx) {
    if (count != 1 || args[0].type != TYPE_STRING) {
        set_error(ctx, "TypeError", "explode expects a string message"); return create_null();
    }
    set_error(ctx, "RuntimeError", "%s", args[0].as.string->str);
    return create_null();
}

static Value builtin_file_open(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_STRING || args[1].type != TYPE_STRING) {
        set_error(ctx, "TypeError", "file_open expects name and mode strings"); return create_null();
    }
    FILE* fp = fopen(args[0].as.string->str, args[1].as.string->str);
    if (!fp) {
        set_error(ctx, "FileError", "Cannot open file '%s'", args[0].as.string->str); return create_null();
    }
    return create_file(fp);
}

static Value builtin_file_read(Value* args, int count, EvalContext* ctx) {
    if (count != 1 || args[0].type != TYPE_FILE || !args[0].as.file->is_open) {
        set_error(ctx, "TypeError", "file_read expects an open file"); return create_null();
    }
    FILE* fp = args[0].as.file->fp;
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* string = malloc(fsize + 1);
    
    // Fix applied here as well for runtime file reads on Windows
    size_t read_bytes = fread(string, 1, fsize, fp);
    string[read_bytes] = '\0';
    
    Value v = create_string(string);
    free(string);
    return v;
}

static Value builtin_file_write(Value* args, int count, EvalContext* ctx) {
    if (count != 2 || args[0].type != TYPE_FILE || !args[0].as.file->is_open || args[1].type != TYPE_STRING) {
        set_error(ctx, "TypeError", "file_write expects open file and string"); return create_null();
    }
    fprintf(args[0].as.file->fp, "%s", args[1].as.string->str);
    return create_null();
}

static Value builtin_file_close(Value* args, int count, EvalContext* ctx) {
    if (count != 1 || args[0].type != TYPE_FILE) {
        set_error(ctx, "TypeError", "file_close expects a file"); return create_null();
    }
    if (args[0].as.file->is_open) {
        fclose(args[0].as.file->fp);
        args[0].as.file->is_open = false;
        args[0].as.file->fp = NULL;
    }
    return create_null();
}

void register_builtins(Env* env) {
    env_set(env, "print", create_builtin(builtin_print, "print"));
    env_set(env, "println", create_builtin(builtin_println, "println"));
    env_set(env, "add", create_builtin(builtin_add, "add"));
    env_set(env, "sub", create_builtin(builtin_sub, "sub"));
    env_set(env, "mul", create_builtin(builtin_mul, "mul"));
    env_set(env, "div", create_builtin(builtin_div, "div"));
    env_set(env, "mod", create_builtin(builtin_mod, "mod"));
    env_set(env, "pow", create_builtin(builtin_pow, "pow"));
    env_set(env, "eq", create_builtin(builtin_eq, "eq"));
    env_set(env, "neq", create_builtin(builtin_neq, "neq"));
    env_set(env, "gt", create_builtin(builtin_gt, "gt"));
    env_set(env, "lt", create_builtin(builtin_lt, "lt"));
    env_set(env, "gte", create_builtin(builtin_gte, "gte"));
    env_set(env, "lte", create_builtin(builtin_lte, "lte"));
    env_set(env, "and", create_builtin(builtin_and, "and"));
    env_set(env, "or", create_builtin(builtin_or, "or"));
    env_set(env, "not", create_builtin(builtin_not, "not"));
    env_set(env, "give_back", create_builtin(builtin_give_back, "give_back"));
    env_set(env, "list", create_builtin(builtin_list, "list"));
    env_set(env, "item", create_builtin(builtin_item, "item"));
    env_set(env, "push", create_builtin(builtin_push, "push"));
    env_set(env, "list_len", create_builtin(builtin_list_len, "list_len"));
    env_set(env, "explode", create_builtin(builtin_explode, "explode"));
    env_set(env, "file_open", create_builtin(builtin_file_open, "file_open"));
    env_set(env, "file_read", create_builtin(builtin_file_read, "file_read"));
    env_set(env, "file_write", create_builtin(builtin_file_write, "file_write"));
    env_set(env, "file_close", create_builtin(builtin_file_close, "file_close"));
}