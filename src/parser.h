#ifndef FUNLANG_PARSER_H
#define FUNLANG_PARSER_H

#include <stdbool.h> // <-- Added this include for safety
#include "lexer.h"
#include "ast.h"

typedef struct {
    TokenList* list;
    int current;
    Arena* arena;
    bool has_error;
    const char* file_name;
} Parser;

AST* parse_program(TokenList* tokens, Arena* arena, const char* filename);

#endif // FUNLANG_PARSER_H