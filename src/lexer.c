#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static void add_token(TokenList* list, TokenType type, const char* text, int len, int line, int col) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity == 0 ? 128 : list->capacity * 2;
        list->tokens = (Token*)realloc(list->tokens, sizeof(Token) * list->capacity);
    }
    Token* t = &list->tokens[list->count++];
    t->type = type;
    t->line = line;
    t->col = col;
    if (text) {
        t->text = (char*)malloc(len + 1);
        strncpy(t->text, text, len);
        t->text[len] = '\0';
    } else {
        t->text = NULL;
    }
}

TokenList tokenize(const char* source) {
    TokenList list = {NULL, 0, 0, false};
    int line = 1;
    int col = 1;
    int i = 0;

    while (source[i] != '\0') {
        char c = source[i];
        
        if (c == '\n') {
            add_token(&list, TOK_NEWLINE, NULL, 0, line, col);
            line++;
            col = 1;
            i++;
            continue;
        }
        
        if (isspace(c)) {
            i++; col++;
            continue;
        }

        // Comments
        if (c == '/' && source[i+1] == '/') {
            while (source[i] != '\n' && source[i] != '\0') {
                i++;
            }
            continue; // let the newline handler catch the \n
        }

        if (isalpha(c) || c == '_') {
            int start = i;
            while (isalnum(source[i]) || source[i] == '_') i++;
            add_token(&list, TOK_IDENTIFIER, &source[start], i - start, line, col);
            col += (i - start);
            continue;
        }

        if (isdigit(c) || (c == '-' && isdigit(source[i+1]))) {
            int start = i;
            if (c == '-') i++;
            while (isdigit(source[i]) || source[i] == '.') i++;
            add_token(&list, TOK_NUMBER, &source[start], i - start, line, col);
            col += (i - start);
            continue;
        }

        if (c == '"') {
            i++; // skip quote
            int start = i;
            while (source[i] != '"' && source[i] != '\0') {
                if (source[i] == '\n') line++;
                i++;
            }
            if (source[i] == '\0') {
                fprintf(stderr, "Lexer Error: Unterminated string at line %d\n", line);
                list.has_error = true;
                break;
            }
            add_token(&list, TOK_STRING, &source[start], i - start, line, col);
            col += (i - start + 2);
            i++; // skip end quote
            continue;
        }

        if (c == '(') { add_token(&list, TOK_LPAREN, NULL, 0, line, col); i++; col++; continue; }
        if (c == ')') { add_token(&list, TOK_RPAREN, NULL, 0, line, col); i++; col++; continue; }
        if (c == ',') { add_token(&list, TOK_COMMA, NULL, 0, line, col); i++; col++; continue; }
        if (c == '=') { add_token(&list, TOK_EQUALS, NULL, 0, line, col); i++; col++; continue; }

        fprintf(stderr, "Lexer Error: Unexpected character '%c' at line %d:%d\n", c, line, col);
        list.has_error = true;
        break;
    }

    add_token(&list, TOK_EOF, NULL, 0, line, col);
    return list;
}

void free_token_list(TokenList* list) {
    for (int i = 0; i < list->count; i++) {
        free(list->tokens[i].text);
    }
    free(list->tokens);
}