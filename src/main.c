#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include "builtins.h"

char* read_file(const char* filename) {
    // Open in "rb" (binary) mode to prevent Windows \r\n translation issues
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: Could not open file '%s'\n", filename);
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char* buf = malloc(size + 1);
    if (!buf) {
        fclose(fp);
        return NULL;
    }
    
    // Store the actual number of bytes read and terminate exactly there
    size_t bytes_read = fread(buf, 1, size, fp);
    buf[bytes_read] = '\0';
    
    fclose(fp);
    return buf;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: funlang <script.fun>\n");
        return 1;
    }
    
    const char* filename = argv[1];
    char* source = read_file(filename);
    if (!source) return 1;
    
    TokenList tokens = tokenize(source);
    if (tokens.has_error) {
        free(source);
        free_token_list(&tokens);
        return 1;
    }
    
    Arena arena;
    arena_init(&arena);
    
    AST* ast = parse_program(&tokens, &arena, filename);
    if (!ast) {
        free(source);
        free_token_list(&tokens);
        arena_free(&arena);
        return 1;
    }
    
    Env* global = create_env(NULL);
    register_builtins(global);
    
    EvalContext ctx;
    init_context(&ctx);
    ctx.global_env = global;
    
    execute_program(ast, &ctx);
    
    if (ctx.has_error) {
        print_error_trace(&ctx);
    }
    
    free_env(global);
    arena_free(&arena);
    free_token_list(&tokens);
    free(source);
    
    return ctx.has_error ? 1 : 0;
}