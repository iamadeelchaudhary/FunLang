#include "error.h"
#include <stdio.h>
#include <string.h>

void init_context(struct EvalContext* ctx) {
    ctx->current_env = NULL;
    ctx->global_env = NULL;
    ctx->has_error = false;
    ctx->error_type[0] = '\0';
    ctx->error_msg[0] = '\0';
    ctx->error_file[0] = '\0';
    ctx->error_line = 0;
    ctx->error_col = 0;
    ctx->returned = false;
    ctx->return_value = create_null();
    ctx->frame_count = 0;
    ctx->step_count = 0;
}

void push_frame(struct EvalContext* ctx, const char* func, const char* file, int line) {
    if (ctx->frame_count >= MAX_FRAMES) {
        set_error(ctx, "StackOverflow", "Maximum call depth of %d exceeded.", MAX_FRAMES);
        return;
    }
    CallFrame* f = &ctx->frames[ctx->frame_count++];
    strncpy(f->func_name, func ? func : "<anonymous>", sizeof(f->func_name) - 1);
    f->func_name[sizeof(f->func_name) - 1] = '\0';
    strncpy(f->file_name, file ? file : "<unknown>", sizeof(f->file_name) - 1);
    f->file_name[sizeof(f->file_name) - 1] = '\0';
    f->line = line;
}

void pop_frame(struct EvalContext* ctx) {
    if (ctx->frame_count > 0) {
        ctx->frame_count--;
    }
}

void set_error(struct EvalContext* ctx, const char* type, const char* format, ...) {
    if (ctx->has_error) return; // Retain the first error thrown
    ctx->has_error = true;
    strncpy(ctx->error_type, type, sizeof(ctx->error_type) - 1);
    ctx->error_type[sizeof(ctx->error_type) - 1] = '\0';
    
    va_list args;
    va_start(args, format);
    vsnprintf(ctx->error_msg, sizeof(ctx->error_msg), format, args);
    va_end(args);
    
    if (ctx->frame_count > 0) {
        CallFrame* f = &ctx->frames[ctx->frame_count - 1];
        strncpy(ctx->error_file, f->file_name, sizeof(ctx->error_file) - 1);
        ctx->error_file[sizeof(ctx->error_file) - 1] = '\0';
        ctx->error_line = f->line;
    }
}

void print_error_trace(struct EvalContext* ctx) {
    if (!ctx->has_error) return;
    
    fprintf(stderr, "\n[FunLang Error] %s at line %d in '%s'\n", 
            ctx->error_type, ctx->error_line, ctx->error_file);
    fprintf(stderr, "Details: %s\n", ctx->error_msg);
    fprintf(stderr, "Stack trace:\n");
    
    for (int i = ctx->frame_count - 1; i >= 0; i--) {
        CallFrame* f = &ctx->frames[i];
        fprintf(stderr, "  at %s -> %s:%d\n", f->func_name, f->file_name, f->line);
    }
    fprintf(stderr, "\n");
}