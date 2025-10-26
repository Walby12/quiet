#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

typedef struct Variable Variable;

typedef struct CodeGen {
	FILE *out;
    int temp_counter;
    int label_counter;
} CodeGen;

CodeGen* codegen_init(const char *filename);
void codegen_start_function(CodeGen *cg, const char *name);
void codegen_putchar(CodeGen *cg, int value);
void codegen_putchar_variable(CodeGen *cg, const char *value);
void codegen_return(CodeGen *cg, int value);
void codegen_variable(CodeGen *cg, Variable *v);
void codegen_variable_reassing(CodeGen *cg, Variable *v, int value);
void codegen_end_function(CodeGen *cg);
void codegen_finish(CodeGen *cg);

#endif
