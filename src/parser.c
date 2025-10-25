#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

Variable *symbol_table = NULL;

void add_variable(const char *name) {
	Variable *v;

	HASH_FIND_STR(symbol_table, name, v);
	if (v != NULL) {
		return;
	}

	v = malloc(sizeof(Variable));
	strcpy(v->name, name);

	HASH_ADD_STR(symbol_table, name, v);
}

int variable_exists(const char *name) {
	Variable *v;
	HASH_FIND_STR(symbol_table, name, v);
	return (v != NULL);
}

Variable* get_variable(const char *name) {
	Variable *v;
	HASH_FIND_STR(symbol_table, name, v);
	return v;
}

void delete_variable(const char *name) {
	Variable *v;
	HASH_FIND_STR(symbol_table, name, v);
	if (v != NULL) {
		HASH_DEL(symbol_table, v);
		free(v);
	}
}

int count_variables() {
	return HASH_COUNT(symbol_table);
}

void print_all_variables() {
	Variable *v, *tmp;
	HASH_ITER(hh, symbol_table, v, tmp) {
		printf("%s\n", v->name);
	}
}

void cleanup_symbol_table() {
	Variable *v, *tmp;
	HASH_ITER(hh, symbol_table, v, tmp) {
		HASH_DEL(symbol_table, v);
		free(v);
	}
}

char buff[14];

void parse(Lexer *lex) {
	switch (lex->cur_tok) {
		case FN:
			get_next_tok(lex);
			parse_expect(lex, ID);
			printf("public %s\n", lex_id);
			printf("%s:\n", lex_id);
			get_next_tok(lex);
			parse_expect(lex, OPEN_PAREN);
			get_next_tok(lex);
			parse_expect(lex, CLOSE_PAREN);
			get_next_tok(lex);
			parse_expect(lex, ARROW);
			get_next_tok(lex);
			parse_expect(lex, ID);
			if ((strcmp(lex_id, "int") != 0)) {
				printf("ERROR: [%d,%d]: unknown return type %s\n", 
						cur_line, cur_col + 1, lex_id);
				exit(1);
			}
			get_next_tok(lex);
			parse_expect(lex, OPEN_CURLY);
			get_next_tok(lex);  // Fixed: was get_next_tol
			
			if (strcmp(lex_id, "putchar") == 0) {
				get_next_tok(lex);
				parse_expect(lex, OPEN_PAREN);
				get_next_tok(lex);
				if (lex->cur_tok == NUM) {
					printf("\tputchar %d\n", lex_num);
				}
				// TODO: Add rest of putchar logic
			}
			// TODO: Add rest of function body parsing
			break;
		default:
			printf("todo\n");
			exit(0);
	}
}

int parse_expect(Lexer *lex, TokenType t) {
	if (lex->cur_tok != t) {
		printf("ERROR [%d,%d]: expected %s but got %s\n", cur_line, cur_col + 1, 
				to_string(t), to_string(lex->cur_tok));
		exit(1);
	}
	return 1;
}

char* to_string(TokenType t) {
	switch (t) {
		case ID:
			return lex_id;
		case NUM:
			sprintf(buff, "%d", lex_num);
			return buff;
		case OPEN_PAREN:
			return "(";
		case CLOSE_PAREN:
			return ")";
		case OPEN_CURLY:
			return "{";
		case CLOSE_CURLY:
			return "}";
		case SEMICOLON:
			return ";";
		case ARROW:
			return "->";
		case FN:
			return "function";
		case RETURN:
			return "return";
		case STR_END:
			return "eof";
		default:
			return "unknow token";
	}
}
