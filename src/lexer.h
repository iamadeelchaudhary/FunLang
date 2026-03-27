#ifndef FUNLANG_LEXER_H
#define FUNLANG_LEXER_H

#include <stdbool.h> // <-- Added this include

typedef enum {
    TOK_IDENTIFIER,
    TOK_STRING,
    TOK_NUMBER,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_COMMA,
    TOK_EQUALS,
    TOK_NEWLINE,
    TOK_EOF,
    TOK_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char* text; // dynamically allocated, must be freed
    int line;
    int col;
} Token;

typedef struct {
    Token* tokens;
    int count;
    int capacity;
    bool has_error;
} TokenList;

TokenList tokenize(const char* source);
void free_token_list(TokenList* list);

#endif // FUNLANG_LEXER_H