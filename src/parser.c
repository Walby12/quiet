#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codegen.h"

Variable *symbol_table = NULL;
char var_dest[1024];
char func_ret[1024];
char buff[14];

void add_variable_int(const char *name, int value, const char *type) {
	Variable *v;

	HASH_FIND_STR(symbol_table, name, v);
	if (v != NULL) {
		return;
	}

	v = malloc(sizeof(Variable));
	strcpy(v->name, name);
	v->value = value;
	strcpy(v->type, type);

	HASH_ADD_STR(symbol_table, name, v);
}

void add_variable_str(const char *name, const char *value, const char *type) {
	Variable *v;

	HASH_FIND_STR(symbol_table, name, v);
	if (v != NULL) {
		return;
	}

	v = malloc(sizeof(Variable));
	strcpy(v->name, name);
	strcpy(v->value_str, value);
	strcpy(v->type, type);

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
    	printf("%-20s %d %s\n", v->name, v->value, v->type);
    	printf("Total: %d variables\n", HASH_COUNT(symbol_table));
	}
}

void cleanup_symbol_table() {
	Variable *v, *tmp;
	HASH_ITER(hh, symbol_table, v, tmp) {
		HASH_DEL(symbol_table, v);
		free(v);
	}
}

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
            cur_line, cur_col, lex_id);
        exit(1);
    }
	strcpy(func_ret, lex_id);

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
                } else if (lex->cur_tok == ID) {
					if (variable_exists(lex_id)) {
						Variable *v = get_variable(lex_id);
						if (strcmp(v->type, "int") == 0) {
							codegen_putchar_variable(cg, lex_id);
							get_next_tok(lex);
					} else {
						printf("ERROR [%d,%d]: the type of variable %s must be int for putchar\n", cur_line, cur_col, lex_id);
						exit(1);
					}
					} else {
						printf("ERROR [%d,%d]: unknow variable: %s\n", cur_line, cur_col, lex_id);
						exit(1);
					}
				} else {
                    printf("ERROR [%d,%d]: expected number or a variable in putchar\n", cur_line, cur_col);
                    exit(1);
                }
                
                parse_expect(lex, CLOSE_PAREN);
            	get_next_tok(lex);
                parse_expect(lex, SEMICOLON);
                get_next_tok(lex);
            } else if (strcmp(lex_id, "int") == 0) {
		    	get_next_tok(lex);
		    	strcpy(var_dest, lex_id);
		    	parse_expect(lex, ID);
		    
		    	get_next_tok(lex);
		    	parse_expect(lex, EQUALS);

		    	get_next_tok(lex);
		    	parse_expect(lex, NUM);
		    	int val = lex_num;
			
		    	get_next_tok(lex);
		    	parse_expect(lex, SEMICOLON);
		    	get_next_tok(lex);
		    	add_variable_int(var_dest, val, "int");
		    	codegen_variable_int(cg, get_variable(var_dest));
				str_index++;
	    	} else if (strcmp(lex_id, "str") == 0) {
				get_next_tok(lex);
				strcpy(var_dest, lex_id);
				parse_expect(lex, ID);

				get_next_tok(lex);
				parse_expect(lex, EQUALS);

				get_next_tok(lex);
				parse_expect(lex, STRING);
				char *val = lex_str;

				get_next_tok(lex);
				parse_expect(lex, SEMICOLON);
				get_next_tok(lex);
				add_variable_str(var_dest, val, "str");
				append_string(get_variable(var_dest), val);
				codegen_variable_str(cg, get_variable(var_dest));
			} else {
				strcpy(var_dest,lex_id);
				if (!variable_exists(var_dest)) {	
                	printf("ERROR [%d,%d]: unknown operation: %s\n", cur_line, cur_col, lex_id);
                	exit(1);
				} else {
					get_next_tok(lex);
					if (lex->cur_tok == EQUALS) {
						get_next_tok(lex);
						Variable *v = get_variable(var_dest);
						if (v == NULL) {
							printf("ERROR [%d,%d]: variable not inizialized\n", cur_line, cur_col);
							exit(1);
						}

						if (lex->cur_tok == NUM && (strcmp(v->type, "int") == 0)) {
							int value = lex_num;
							get_next_tok(lex);
							parse_expect(lex, SEMICOLON);
							get_next_tok(lex);
							codegen_variable_reassign_int(cg, v, value);
						} else if (lex->cur_tok == NUM && (strcmp(v->type, "int") != 0)) {
							printf("ERROR [%d,%d]: tried to assing an int value to an %s variable\n", cur_line, cur_col, v->type);
							exit(1);
						} else if (lex->cur_tok != NUM && strcmp(v->type, "int") == 0) {
							printf("ERROR [%d,%d]: tried to assign an %s value to a int variable\n", cur_line, cur_col, to_string(lex->cur_tok));
							exit(1);
						} else if (lex->cur_tok == STRING && strcmp(v->type, "str") == 0) {
							char *val = lex_str;
							get_next_tok(lex);
							parse_expect(lex, SEMICOLON);
							get_next_tok(lex);
							add_variable_str(v->name, val, "str");
							append_string(v, val);
							codegen_variable_reassign_str(cg, v);	
						} else if (lex->cur_tok == STRING && strcmp(v->type, "str") != 0) {
							printf("ERROR [%d,%d]: tried to assign a string value to an %s variable\n", cur_line, cur_col, v->type);
							exit(1);
						} else if (lex->cur_tok != STRING && strcmp(v->type, "str") == 0) {
							printf("ERROR [%d,%d]: tried to assign an %s value to a string variable\n", cur_line, cur_col, to_string(lex->cur_tok));
							exit(1);
						}
					}
				}
            }
        } else if (lex->cur_tok == RETURN) {
            get_next_tok(lex);
            if (lex->cur_tok == NUM && strcmp(func_ret, "int") == 0) {
		    codegen_return(cg, lex_num);
			get_next_tok(lex);
        	} else if (lex->cur_tok == NUM && strcmp(func_ret, "int") != 0) {
                printf("ERROR [%d,%d]: returned an int from a non int returning function\n", cur_line, cur_col);
				exit(1);
            } else if (lex->cur_tok != NUM && strcmp(func_ret, "int") == 0) {
				printf("ERROR [%d,%d]: returned an non int from an int returning funtion\n", cur_line, cur_col);
				exit(1);
			}
            parse_expect(lex, SEMICOLON);
            get_next_tok(lex);
        } else {
            printf("ERROR [%d,%d]: unexpected token %s\n", cur_line, cur_col, to_string(lex->cur_tok));
            exit(1);
        }
    }
    
    parse_expect(lex, CLOSE_CURLY);
    codegen_end_function(cg);
}

int parse_expect(Lexer *lex, TokenType t) {
	if (lex->cur_tok != t) {
		printf("ERROR [%d,%d]: expected %s but got %s\n", cur_line, cur_col, 
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
		case STRING:
			return "\"";
		default:
			return "unknow token";
	}
}
