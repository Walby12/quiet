#ifndef PARSER_H
#define PARSER_H

#include "uthash.h"
#include "lexer.h"

typedef struct CodeGen CodeGen;

typedef struct Variable {
    char name[256];
    int value;
    char value_str[1024];
    char type[16];
    char data_label[64];
    UT_hash_handle hh;
} Variable;

extern Variable *symbol_table;
extern char dest[1024];
extern char func_ret[1024];
extern int func_ret_c;
extern int func_buff;
extern char **func_names;
extern int func_index; 

void add_variable_int(const char *name, int value, const char *type);
void add_variable_str(const char *name, const char *value, const char *type);
int variable_exists(const char *name);
Variable* get_variable(const char *name);
void delete_variable(const char *name);
int count_variables();
void print_all_variables();
void cleanup_symbol_table();

int parse_primary(Lexer *lex, CodeGen *cg);
int parse_expression(Lexer *lex, CodeGen *cg);
int parse_term(Lexer *lex, CodeGen *cg);
int parse_factor(Lexer *lex, CodeGen *cg);
void parse(Lexer *lex, CodeGen *cg);
int parse_expect(Lexer *lex, TokenType t);
char* to_string(TokenType t);

#endif
