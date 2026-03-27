#ifndef FUNLANG_EVAL_H
#define FUNLANG_EVAL_H

#include "ast.h"
#include "env.h"
#include "error.h"

void init_evaluator(void);
Value eval(AST* node, Env* env, EvalContext* ctx);
void execute_program(AST* root, EvalContext* ctx);

#endif // FUNLANG_EVAL_H