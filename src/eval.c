#include "eval.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static Value eval_func_call(AST* node, Env* env, EvalContext* ctx);

Value eval(AST* node, Env* env, EvalContext* ctx) {
    if (ctx->has_error) return create_null();
    
    ctx->step_count++;
    if (ctx->step_count > MAX_STEPS) {
        set_error(ctx, "LimitExceeded", "Execution step limit reached");
        return create_null();
    }
    
    switch (node->type) {
        case AST_PROGRAM: {
            Value res = create_null();
            for (int i = 0; i < node->arg_count; i++) {
                release_val(res);
                res = eval(node->args[i], env, ctx);
                if (ctx->has_error || ctx->returned) break;
            }
            return res;
        }
        case AST_LITERAL_STRING:
        case AST_LITERAL_NUMBER:
        case AST_LITERAL_BOOL:
        case AST_LITERAL_NULL:
            return retain_val(node->literal);
            
        case AST_IDENTIFIER:
            set_error(ctx, "SyntaxError", "Cannot evaluate raw identifier '%s'. Use var(%s)", node->name, node->name);
            return create_null();
            
        case AST_VAR: {
            Value val;
            if (env_get(env, node->name, &val)) {
                return retain_val(val);
            }
            set_error(ctx, "NameError", "Variable '%s' is not defined", node->name);
            return create_null();
        }
            
        case AST_ASSIGN: {
            Value val = eval(node->args[0], env, ctx);
            if (ctx->has_error) return create_null();
            env_set(env, node->name, val);
            return val;
        }
            
        case AST_FUNC_CALL:
            return eval_func_call(node, env, ctx);
    }
    return create_null();
}

static Value eval_func_call(AST* node, Env* env, EvalContext* ctx) {
    push_frame(ctx, node->name, node->file, node->line);
    
    // Check Special Forms
    if (strcmp(node->name, "iff") == 0) {
        if (node->arg_count != 3) { set_error(ctx, "ArgError", "iff expects 3 arguments"); pop_frame(ctx); return create_null(); }
        Value cond = eval(node->args[0], env, ctx);
        if (ctx->has_error) { pop_frame(ctx); return create_null(); }
        
        bool is_true = (cond.type == TYPE_BOOL && cond.as.boolean);
        release_val(cond);
        
        Value res = eval(is_true ? node->args[1] : node->args[2], env, ctx);
        pop_frame(ctx);
        return res;
    }
    
    if (strcmp(node->name, "loop_while") == 0) {
        if (node->arg_count != 2) { set_error(ctx, "ArgError", "loop_while expects 2 arguments"); pop_frame(ctx); return create_null(); }
        Value res = create_null();
        while (true) {
            Value cond = eval(node->args[0], env, ctx);
            if (ctx->has_error) break;
            bool is_true = (cond.type == TYPE_BOOL && cond.as.boolean);
            release_val(cond);
            if (!is_true) break;
            
            release_val(res);
            res = eval(node->args[1], env, ctx);
            if (ctx->has_error || ctx->returned) break;
        }
        pop_frame(ctx);
        return res;
    }
    
    if (strcmp(node->name, "loop_for") == 0) {
        if (node->arg_count != 4 || node->args[0]->type != AST_VAR) {
            set_error(ctx, "ArgError", "loop_for expects var(name), start, end, body");
            pop_frame(ctx); return create_null();
        }
        const char* var_name = node->args[0]->name;
        Value start_v = eval(node->args[1], env, ctx);
        Value end_v = eval(node->args[2], env, ctx);
        if (ctx->has_error) { pop_frame(ctx); return create_null(); }
        
        if (start_v.type != TYPE_NUMBER || end_v.type != TYPE_NUMBER) {
            set_error(ctx, "TypeError", "loop_for bounds must be numbers");
            release_val(start_v); release_val(end_v);
            pop_frame(ctx); return create_null();
        }
        
        double start = start_v.as.number;
        double end = end_v.as.number;
        release_val(start_v); release_val(end_v);
        
        Env* loop_env = create_env(env);
        Value res = create_null();
        for (double i = start; i < end; i++) {
            env_set(loop_env, var_name, create_number(i));
            release_val(res);
            res = eval(node->args[3], loop_env, ctx);
            if (ctx->has_error || ctx->returned) break;
        }
        free_env(loop_env);
        pop_frame(ctx);
        return res;
    }
    
    if (strcmp(node->name, "do") == 0 || strcmp(node->name, "otherwise") == 0) {
        Value res = create_null();
        for (int i = 0; i < node->arg_count; i++) {
            release_val(res);
            res = eval(node->args[i], env, ctx);
            if (ctx->has_error || ctx->returned) break;
        }
        pop_frame(ctx);
        return res;
    }
    
    if (strcmp(node->name, "cook") == 0) {
        if (node->arg_count != 3 || node->args[0]->type != AST_IDENTIFIER || 
            node->args[1]->type != AST_FUNC_CALL || strcmp(node->args[1]->name, "args") != 0) {
            set_error(ctx, "SyntaxError", "cook syntax: cook(name, args(a,b), do(...))");
            pop_frame(ctx); return create_null();
        }
        const char* func_name = node->args[0]->name;
        Value func_val = create_func(node->args[2], node->args[1], env);
        env_set(env, func_name, func_val);
        release_val(func_val);
        pop_frame(ctx);
        return create_null();
    }
    
    if (strcmp(node->name, "serve") == 0) {
        if (node->arg_count < 1 || node->args[0]->type != AST_IDENTIFIER) {
            set_error(ctx, "SyntaxError", "serve syntax: serve(func_name, arg1...)");
            pop_frame(ctx); return create_null();
        }
        const char* func_name = node->args[0]->name;
        Value func_val;
        if (!env_get(env, func_name, &func_val) || func_val.type != TYPE_FUNCTION) {
            set_error(ctx, "NameError", "'%s' is not a defined function", func_name);
            pop_frame(ctx); return create_null();
        }
        
        FuncVal* fn = func_val.as.func;
        int expected_args = fn->args_node->arg_count;
        if (node->arg_count - 1 != expected_args) {
            set_error(ctx, "ArgError", "Function '%s' expects %d args", func_name, expected_args);
            pop_frame(ctx); return create_null();
        }
        
        // Evaluate args BEFORE pushing new env
        Value* evaled_args = malloc(sizeof(Value) * expected_args);
        for (int i = 0; i < expected_args; i++) {
            evaled_args[i] = eval(node->args[i+1], env, ctx);
            if (ctx->has_error) {
                for (int j = 0; j <= i; j++) release_val(evaled_args[j]);
                free(evaled_args);
                pop_frame(ctx); return create_null();
            }
        }
        
        Env* call_env = create_env(fn->captured_env);
        for (int i = 0; i < expected_args; i++) {
            const char* arg_name = fn->args_node->args[i]->name;
            env_set(call_env, arg_name, evaled_args[i]);
            release_val(evaled_args[i]);
        }
        free(evaled_args);
        
        Value res = eval(fn->body, call_env, ctx);
        free_env(call_env);
        
        if (ctx->returned) {
            ctx->returned = false; // consume return
            res = retain_val(ctx->return_value);
            release_val(ctx->return_value);
            ctx->return_value = create_null();
        }
        
        pop_frame(ctx);
        return res;
    }
    
    if (strcmp(node->name, "attempt") == 0) {
        if (node->arg_count != 2) { set_error(ctx, "ArgError", "attempt expects do(), rescue()"); pop_frame(ctx); return create_null(); }
        Value res = eval(node->args[0], env, ctx);
        if (ctx->has_error) {
            char err_buf[256];
            strncpy(err_buf, ctx->error_msg, 256);
            ctx->has_error = false; // catch
            
            AST* rescue_node = node->args[1];
            if (rescue_node->type == AST_FUNC_CALL && strcmp(rescue_node->name, "rescue") == 0 && rescue_node->arg_count == 2) {
                Env* rescue_env = create_env(env);
                Value err_val = create_string(err_buf);
                env_set(rescue_env, rescue_node->args[0]->name, err_val);
                release_val(err_val);
                res = eval(rescue_node->args[1], rescue_env, ctx);
                free_env(rescue_env);
            }
        }
        pop_frame(ctx);
        return res;
    }

    // Normal Builtin / function evaluation
    Value func_def;
    if (!env_get(env, node->name, &func_def)) {
        set_error(ctx, "NameError", "Undefined function '%s'", node->name);
        pop_frame(ctx); return create_null();
    }
    
    if (func_def.type != TYPE_BUILTIN) {
        set_error(ctx, "TypeError", "'%s' is not callable", node->name);
        pop_frame(ctx); return create_null();
    }
    
    Value* evaled_args = NULL;
    if (node->arg_count > 0) {
        evaled_args = malloc(sizeof(Value) * node->arg_count);
        for (int i = 0; i < node->arg_count; i++) {
            evaled_args[i] = eval(node->args[i], env, ctx);
            if (ctx->has_error) {
                for (int j = 0; j <= i; j++) release_val(evaled_args[j]);
                free(evaled_args);
                pop_frame(ctx); return create_null();
            }
        }
    }
    
    Value result = func_def.as.builtin.func(evaled_args, node->arg_count, ctx);
    
    for (int i = 0; i < node->arg_count; i++) release_val(evaled_args[i]);
    if (evaled_args) free(evaled_args);
    
    pop_frame(ctx);
    return result;
}

void execute_program(AST* root, EvalContext* ctx) {
    eval(root, ctx->global_env, ctx);
}