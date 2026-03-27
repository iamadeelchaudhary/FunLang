#ifndef FUNLANG_ERROR_H
#define FUNLANG_ERROR_H

#include <stdbool.h>
#include <stdarg.h>
#include "value.h" // Brings in Value type and typedefs

#define MAX_FRAMES 1000
#define MAX_STEPS 10000000

// Forward declare struct Env to prevent circular dependencies
struct Env;

typedef struct CallFrame {
    char func_name[64];
    char file_name[128];
    int line;
} CallFrame;

// Define the structure for EvalContext
struct EvalContext {
    struct Env* current_env;
    struct Env* global_env;
    
    bool has_error;
    char error_type[64];
    char error_msg[256];
    char error_file[128];
    int error_line;
    int error_col;
    
    bool returned;
    Value return_value;
    
    CallFrame frames[MAX_FRAMES];
    int frame_count;
    int step_count;
};

// Function prototypes using the struct explicitly to guarantee visibility
void init_context(struct EvalContext* ctx);
void push_frame(struct EvalContext* ctx, const char* func, const char* file, int line);
void pop_frame(struct EvalContext* ctx);
void set_error(struct EvalContext* ctx, const char* type, const char* format, ...);
void print_error_trace(struct EvalContext* ctx);

#endif // FUNLANG_ERROR_H