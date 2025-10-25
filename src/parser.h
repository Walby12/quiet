#ifndef PARSER_H
#define PARSER_H

#include "uthash.h"
#include "lexer.h"

// Forward declaration to avoid circular include
typedef struct CodeGen CodeGen;

// ✅ Give the struct a name ("Variable") so forward declarations work properly
typedef struct Variable {
    char name[256];
    int value;
    UT_hash_handle hh;
} Variable;

extern Variable *symbol_table;
extern char dest[1024];
extern char buff[14];

void add_variable(const char *name, int value);
int variable_exists(const char *name);
Variable* get_variable(const char *name);
void delete_variable(const char *name);
int count_variables();
void print_all_variables();
void cleanup_symbol_table();

void parse(Lexer *lex, CodeGen *cg);
int parse_expect(Lexer *lex, TokenType t);
char* to_string(TokenType t);

#endif

