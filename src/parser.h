#ifndef PARSER_H
#define PARSER_H

#ifndef PARSER_H
#define PARSER_H

#include "uthash.h"
#include "lexer.h"

typedef struct {
	char name[256];
	UT_hash_handle hh;
} Variable;

extern Variable *symbol_table;

void add_variable(const char *name);
int variable_exists(const char *name);
Variable* get_variable(const char *name);
void delete_variable(const char *name);
int count_variables();
void print_all_variables();
void cleanup_symbol_table();

extern char buff[14];

void parse(Lexer *lex);
int parse_expect(Lexer *lex, TokenType t);
char* to_string(TokenType t);

#endif
