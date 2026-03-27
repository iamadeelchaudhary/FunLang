#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static Token* peek(Parser* p) {
    return &p->list->tokens[p->current];
}

static Token* peek_next(Parser* p, int offset) {
    if (p->current + offset >= p->list->count) return &p->list->tokens[p->list->count - 1];
    return &p->list->tokens[p->current + offset];
}

static void skip_newlines(Parser* p) {
    while (peek(p)->type == TOK_NEWLINE) p->current++;
}

static Token* advance(Parser* p) {
    skip_newlines(p);
    Token* t = peek(p);
    if (t->type != TOK_EOF) p->current++;
    return t;
}

static bool match(Parser* p, TokenType type) {
    skip_newlines(p);
    if (peek(p)->type == type) {
        p->current++;
        return true;
    }
    return false;
}

static AST* parse_expression(Parser* p);
static AST* parse_statement(Parser* p);

static AST* create_node(Parser* p, ASTType type, int line, int col) {
    AST* node = (AST*)arena_alloc(p->arena, sizeof(AST));
    node->type = type;
    node->name = NULL;
    node->literal = create_null();
    node->args = NULL;
    node->arg_count = 0;
    node->line = line;
    node->col = col;
    node->file = arena_strdup(p->arena, p->file_name);
    return node;
}

static AST* parse_expression(Parser* p) {
    skip_newlines(p);
    Token* t = advance(p);
    
    if (t->type != TOK_IDENTIFIER) {
        fprintf(stderr, "Parse Error: Expected identifier at line %d:%d\n", t->line, t->col);
        p->has_error = true;
        return create_node(p, AST_LITERAL_NULL, t->line, t->col);
    }
    
    int line = t->line;
    int col = t->col;
    char* name = t->text;
    
    // Raw identifier checking
    skip_newlines(p);
    if (peek(p)->type != TOK_LPAREN) {
        AST* node = create_node(p, AST_IDENTIFIER, line, col);
        node->name = arena_strdup(p->arena, name);
        return node;
    }
    
    advance(p); // Consume '('
    
    // Check built-in literal functions
    if (strcmp(name, "string") == 0) {
        skip_newlines(p);
        Token* arg = advance(p);
        if (arg->type != TOK_STRING) {
            fprintf(stderr, "Parse Error: string() expects a string literal at line %d\n", arg->line);
            p->has_error = true;
        }
        match(p, TOK_RPAREN);
        AST* node = create_node(p, AST_LITERAL_STRING, line, col);
        node->literal = create_string(arg->text);
        return node;
    }
    else if (strcmp(name, "number") == 0) {
        skip_newlines(p);
        Token* arg = advance(p);
        if (arg->type != TOK_NUMBER) {
            fprintf(stderr, "Parse Error: number() expects a number literal at line %d\n", arg->line);
            p->has_error = true;
        }
        match(p, TOK_RPAREN);
        AST* node = create_node(p, AST_LITERAL_NUMBER, line, col);
        node->literal = create_number(atof(arg->text));
        return node;
    }
    else if (strcmp(name, "bool") == 0) {
        skip_newlines(p);
        Token* arg = advance(p);
        bool val = false;
        if (arg->type == TOK_IDENTIFIER && strcmp(arg->text, "true") == 0) val = true;
        else if (arg->type == TOK_IDENTIFIER && strcmp(arg->text, "false") == 0) val = false;
        else {
            fprintf(stderr, "Parse Error: bool() expects true or false at line %d\n", arg->line);
            p->has_error = true;
        }
        match(p, TOK_RPAREN);
        AST* node = create_node(p, AST_LITERAL_BOOL, line, col);
        node->literal = create_bool(val);
        return node;
    }
    else if (strcmp(name, "null") == 0) {
        match(p, TOK_RPAREN);
        return create_node(p, AST_LITERAL_NULL, line, col);
    }
    else if (strcmp(name, "var") == 0) {
        skip_newlines(p);
        Token* arg = advance(p);
        if (arg->type != TOK_IDENTIFIER) {
            fprintf(stderr, "Parse Error: var() expects an identifier at line %d\n", arg->line);
            p->has_error = true;
        }
        match(p, TOK_RPAREN);
        AST* node = create_node(p, AST_VAR, line, col);
        node->name = arena_strdup(p->arena, arg->text);
        return node;
    }
    
    // Normal function call
    AST* node = create_node(p, AST_FUNC_CALL, line, col);
    node->name = arena_strdup(p->arena, name);
    
    AST* args[256];
    int arg_count = 0;
    
    skip_newlines(p);
    while (peek(p)->type != TOK_RPAREN && peek(p)->type != TOK_EOF) {
        // FIX: Allow full statements (like variable assignments) as arguments!
        args[arg_count++] = parse_statement(p); 
        skip_newlines(p);
        if (peek(p)->type == TOK_COMMA) {
            advance(p);
            skip_newlines(p);
        } else if (peek(p)->type != TOK_RPAREN) {
            fprintf(stderr, "Parse Error: Expected ',' or ')' in function arguments at line %d\n", peek(p)->line);
            p->has_error = true;
            break;
        }
    }
    match(p, TOK_RPAREN);
    
    if (arg_count > 0) {
        node->args = (AST**)arena_alloc(p->arena, sizeof(AST*) * arg_count);
        memcpy(node->args, args, sizeof(AST*) * arg_count);
        node->arg_count = arg_count;
    }
    return node;
}

static AST* parse_statement(Parser* p) {
    skip_newlines(p);
    
    // Fixed lookahead for assignment: var(IDENTIFIER) =
    if (peek_next(p, 0)->type == TOK_IDENTIFIER && strcmp(peek_next(p, 0)->text, "var") == 0 &&
        peek_next(p, 1)->type == TOK_LPAREN &&
        peek_next(p, 2)->type == TOK_IDENTIFIER &&
        peek_next(p, 3)->type == TOK_RPAREN &&
        peek_next(p, 4)->type == TOK_EQUALS) {
        
        Token* var_tok = advance(p); // var
        advance(p); // (
        Token* name_tok = advance(p); // name
        advance(p); // )
        advance(p); // =
        
        AST* assign = create_node(p, AST_ASSIGN, var_tok->line, var_tok->col);
        assign->name = arena_strdup(p->arena, name_tok->text);
        
        assign->args = (AST**)arena_alloc(p->arena, sizeof(AST*));
        
        // FIX: Allow chained assignments like var(a) = var(b) = 5
        assign->args[0] = parse_statement(p); 
        assign->arg_count = 1;
        
        return assign;
    }
    
    return parse_expression(p);
}

AST* parse_program(TokenList* tokens, Arena* arena, const char* filename) {
    Parser p = {tokens, 0, arena, false, filename};
    
    AST* root = create_node(&p, AST_PROGRAM, 1, 1);
    AST* stmts[1024];
    int count = 0;
    
    skip_newlines(&p);
    while (peek(&p)->type != TOK_EOF && count < 1024) {
        stmts[count++] = parse_statement(&p);
        skip_newlines(&p);
    }
    
    if (count > 0) {
        root->args = (AST**)arena_alloc(arena, sizeof(AST*) * count);
        memcpy(root->args, stmts, sizeof(AST*) * count);
        root->arg_count = count;
    }
    
    if (p.has_error) return NULL;
    return root;
}