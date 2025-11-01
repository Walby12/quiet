#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "parser.h"

extern int str_index;

typedef struct Variable Variable;

typedef struct CodeGen {
	FILE *out;
    int temp_counter;
    int label_counter;
} CodeGen;

typedef struct String {
    struct Variable *v;
    char label[64];
    char value[1024];
    struct String *next;
} String;

CodeGen* codegen_init(const char *filename);
void codegen_start_function(CodeGen *cg, const char *name);
void codegen_putchar(CodeGen *cg, int value);
void codegen_putchar_variable(CodeGen *cg, const char *value);
void codegen_return(CodeGen *cg, int value);
void codegen_variable_int(CodeGen *cg, Variable *v);
void codegen_variable_str(CodeGen *cg, Variable *v);
void codegen_variable_reassign_int(CodeGen *cg, Variable *v, int value);
void codegen_variable_reassign_str(CodeGen *cg, Variable *v);
void codegen_end_function(CodeGen *cg);
void codegen_emit_strings(CodeGen *cg); 
void codegen_finish(CodeGen *cg);
void append_string(Variable *v, const char *value);
void print_strings();

#endif
