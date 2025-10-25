#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codegen.h"

Variable *symbol_table = NULL;
char var_dest[1024];

void add_variable(const char *name, int value) {
	Variable *v;

	HASH_FIND_STR(symbol_table, name, v);
	if (v != NULL) {
		return;
	}

	v = malloc(sizeof(Variable));
	strcpy(v->name, name);
	v->value = value;

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
    
    printf("Variables:\n");
    printf("%-20s %s\n", "Name", "Value");
    printf("----------------------------------------\n");
    
    HASH_ITER(hh, symbol_table, v, tmp) {
        printf("%-20s %d\n", v->name, v->value);
    }
    
    printf("Total: %d variables\n", HASH_COUNT(symbol_table));
}

void cleanup_symbol_table() {
	Variable *v, *tmp;
	HASH_ITER(hh, symbol_table, v, tmp) {
		HASH_DEL(symbol_table, v);
		free(v);
	}
}

char buff[14];

void parse(Lexer *lex, CodeGen *cg) {
    get_next_tok(lex);
    
    if (lex->cur_tok != FN) {
        printf("ERROR: expected function definition\n");
        exit(1);
    }
    
    get_next_tok(lex);
    parse_expect(lex, ID);
    
    char func_name[256];
    strcpy(func_name, lex_id);
    
    codegen_start_function(cg, func_name);
    
    get_next_tok(lex);
    parse_expect(lex, OPEN_PAREN);
    get_next_tok(lex);
    parse_expect(lex, CLOSE_PAREN);
    get_next_tok(lex);
    parse_expect(lex, ARROW);
    get_next_tok(lex);
    parse_expect(lex, ID);
    
    if (strcmp(lex_id, "int") != 0) {
        printf("ERROR: [%d,%d]: unknown return type %s\n", 
                cur_line, cur_col + 1, lex_id);
        exit(1);
    }
    
    get_next_tok(lex);
    parse_expect(lex, OPEN_CURLY);
    get_next_tok(lex);
    
    while (lex->cur_tok != CLOSE_CURLY && lex->cur_tok != STR_END) {
        if (lex->cur_tok == ID) {
            if (strcmp(lex_id, "putchar") == 0) {
                get_next_tok(lex);
                parse_expect(lex, OPEN_PAREN);
                get_next_tok(lex);
                
                if (lex->cur_tok == NUM) {
                    codegen_putchar(cg, lex_num);
                    get_next_tok(lex);
                } else {
                    printf("ERROR: expected number or a variable in putchar\n");
                    exit(1);
                }
                
                parse_expect(lex, CLOSE_PAREN);
                get_next_tok(lex);
                parse_expect(lex, SEMICOLON);
                get_next_tok(lex);
            } else if(strcmp(lex_id, "int") == 0) {
		    get_next_tok(lex);
		    char *var_name = strcpy(var_dest, lex_id);
		    parse_expect(lex, ID);
		    
		    get_next_tok(lex);
		    parse_expect(lex, EQUALS);

		    get_next_tok(lex);
		    parse_expect(lex, NUM);
		    int val = lex_num;
			
		    get_next_tok(lex);
		    parse_expect(lex, SEMICOLON);
		    get_next_tok(lex);
		    add_variable(var_name, val);
		    print_all_variables();
		    codegen_variable(cg, get_variable(var_name));
	    } else {
                printf("ERROR: unknown function %s\n", lex_id);
                exit(1);
            }
        } else if (lex->cur_tok == RETURN) {
            get_next_tok(lex);
            if (lex->cur_tok == NUM) {
                codegen_return(cg, lex_num);
                get_next_tok(lex);
            } else {
                codegen_return(cg, 0);
            }
            parse_expect(lex, SEMICOLON);
            get_next_tok(lex);
        } else {
            printf("ERROR: unexpected token %s\n", to_string(lex->cur_tok));
            exit(1);
        }
    }
    
    parse_expect(lex, CLOSE_CURLY);
    codegen_end_function(cg);
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
		case EQUALS:
			return "=";
		case STR_END:
			return "eof";
		default:
			return "unknow token";
	}
}
