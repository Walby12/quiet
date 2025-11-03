#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codegen.h"

Variable *symbol_table = NULL;
char var_dest[1024];
char func_ret[1024];
int func_ret_c = 1;
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
	if (lex->cur_tok != ID) {
		printf("ERROR [%d,%d]: expected a return type but got %s\n", cur_line, cur_col, to_string(lex->cur_tok));
		exit(1);
	}

	if (strcmp(lex_id, "int") != 0 && strcmp(lex_id, "str") != 0 && strcmp(lex_id, "void") != 0) {
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
            } else if (strcmp(lex_id, "printf") == 0) {
				get_next_tok(lex);
				parse_expect(lex, OPEN_PAREN);
				
				get_next_tok(lex);
				parse_expect(lex, STRING);

				get_next_tok(lex);
				parse_expect(lex, CLOSE_PAREN);

				get_next_tok(lex);
				parse_expect(lex, SEMICOLON);
				//codegen_printf(lex, cg, strdup(lex_str));
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
                parse_expect(lex, ID);
                strcpy(var_dest, lex_id);

                get_next_tok(lex);
                parse_expect(lex, EQUALS);

                get_next_tok(lex);
                parse_expect(lex, STRING);

                char val_copy[1024];
                strncpy(val_copy, lex_str, sizeof(val_copy));
                val_copy[sizeof(val_copy)-1] = '\0';

                add_variable_str(var_dest, val_copy, "str");
                Variable *v = get_variable(var_dest);
                if (v == NULL) {
                    printf("ERROR: failed to create variable %s\n", var_dest);
                    exit(1);
                }
                append_string(v, val_copy);

                get_next_tok(lex);
                parse_expect(lex, SEMICOLON);
                get_next_tok(lex);

                codegen_variable_str(cg, v);
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
                            char val_copy2[1024];
                            strncpy(val_copy2, lex_str, sizeof(val_copy2));
                            val_copy2[sizeof(val_copy2)-1] = '\0';
                            char *val = val_copy2;
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
		    	codegen_return_int(cg, lex_num);
				get_next_tok(lex);
				if (lex->cur_tok == CLOSE_CURLY) {
					func_ret_c = 0;
				} else {
					func_ret_c  = 1;
				}
        	} else if (lex->cur_tok == NUM && strcmp(func_ret, "int") != 0) {
                printf("ERROR [%d,%d]: tried to return an int from a %s returning function\n", cur_line, cur_col, func_ret);
				exit(1);
            }  else if (lex->cur_tok == STRING && strcmp(func_ret, "str") == 0) {
				char val_copy[1024];
                strncpy(val_copy, lex_str, sizeof(val_copy));
                val_copy[sizeof(val_copy)-1] = '\0';
				
				strcpy(var_dest, "ret");
                add_variable_str(var_dest, val_copy, "str");
                Variable *v = get_variable(var_dest);
                if (v == NULL) {
                    printf("ERROR: failed to create variable %s\n", var_dest);
                    exit(1);
                }
                append_string(v, val_copy);
				codegen_return_str(cg, v);
				get_next_tok(lex);
				if (lex->cur_tok == CLOSE_CURLY) {
					func_ret_c = 0;
				} else {
					func_ret_c  = 1;
				}
			} else if (lex->cur_tok == STRING && strcmp(func_ret, "str") != 0) {
				printf("ERROR [%d,%d]: tried to return a string from an %s returning func\n", cur_line, cur_col, func_ret);
				exit(1);
			} else if (lex->cur_tok == SEMICOLON && strcmp(func_ret, "void") == 0) {
				codegen_return_void(cg);
				get_next_tok(lex);
				if (lex->cur_tok == CLOSE_CURLY) {
					func_ret_c = 0;
				} else {
					func_ret_c  = 1;
				}
			} else if (lex->cur_tok == SEMICOLON && strcmp(func_ret, "void") != 0) {
				printf("ERROR [%d,%d]: tried to return void form an %s returning func\n", cur_line, cur_col, func_ret);
				exit(1);
			} else if (lex->cur_tok == ID ) {
				if (strcmp(func_ret, "void") == 0) {
					printf("ERROR [%d,%d]: tried to return a non void from a void returning func\n", cur_line, cur_col);
					exit(1);
				}
				if (variable_exists(lex_id)) {
					Variable *v = get_variable(lex_id);
					if (strcmp(v->type, "int") == 0) {
						codegen_return_int(cg, v->value);
						get_next_tok(lex);

						if (lex->cur_tok == CLOSE_CURLY) {
							func_ret_c = 0;
						} else {
							func_ret_c  = 1;
						}
					} else if (strcmp(v->type, "str") == 0) {
						codegen_return_str(cg, v);
						get_next_tok(lex);
					}
				} else {
					printf("ERROR [%d,%d]: variable %s does not exists\n", cur_line, cur_col, lex_id);
					exit(1);
				}
				
			} 
			if (strcmp(func_ret, "void") != 0) {
            	parse_expect(lex, SEMICOLON);
				get_next_tok(lex);
				if (lex->cur_tok == CLOSE_CURLY) {
					func_ret_c = 0;
				} else {
					func_ret_c  = 1;
				}
        	}
		} else {
            printf("ERROR [%d,%d]: unexpected token %s\n", cur_line, cur_col, to_string(lex->cur_tok));
            exit(1);
        }
    }
	
	if (func_ret_c == 0) {
		parse_expect(lex, CLOSE_CURLY);
		codegen_end_function(cg);
	} else {
		if (lex->cur_tok == RETURN) {
            get_next_tok(lex);
            if (lex->cur_tok == NUM && strcmp(func_ret, "int") == 0) {
		    	codegen_return_int(cg, lex_num);
				get_next_tok(lex);
				if (lex->cur_tok == CLOSE_CURLY) {
					func_ret_c = 0;
				} else {
					func_ret_c  = 1;
				}
        	} else if (lex->cur_tok == NUM && strcmp(func_ret, "int") != 0) {
                printf("ERROR [%d,%d]: tried to return an int from a %s returning function\n", cur_line, cur_col, func_ret);
				exit(1);
            }  else if (lex->cur_tok == STRING && strcmp(func_ret, "str") == 0) {
				char val_copy[1024];
                strncpy(val_copy, lex_str, sizeof(val_copy));
                val_copy[sizeof(val_copy)-1] = '\0';
				
				strcpy(var_dest, "ret");
                add_variable_str(var_dest, val_copy, "str");
                Variable *v = get_variable(var_dest);
                if (v == NULL) {
                    printf("ERROR: failed to create variable %s\n", var_dest);
                    exit(1);
                }
                append_string(v, val_copy);
				codegen_return_str(cg, v);
				get_next_tok(lex);
				if (lex->cur_tok == CLOSE_CURLY) {
					func_ret_c = 0;
				} else {
					func_ret_c  = 1;
				}
			} else if (lex->cur_tok == STRING && strcmp(func_ret, "str") != 0) {
				printf("ERROR [%d,%d]: tried to return a string from an %s returning func\n", cur_line, cur_col, func_ret);
				exit(1);
			} else if (lex->cur_tok == SEMICOLON && strcmp(func_ret, "void") == 0) {
				codegen_return_void(cg);
				get_next_tok(lex);
				if (lex->cur_tok == CLOSE_CURLY) {
					func_ret_c = 0;
				} else {
					func_ret_c  = 1;
				}
			} else if (lex->cur_tok == SEMICOLON && strcmp(func_ret, "void") != 0) {
				printf("ERROR [%d,%d]: tried to return void form an %s returning func\n", cur_line, cur_col, func_ret);
				exit(1);
			} else if (lex->cur_tok == ID ) {
				if (strcmp(func_ret, "void") == 0) {
					printf("ERROR [%d,%d]: tried to return a non void from a void returning func\n", cur_line, cur_col);
					exit(1);
				}
				if (variable_exists(lex_id)) {
					Variable *v = get_variable(lex_id);
					if (strcmp(v->type, "int") == 0) {
						codegen_return_int(cg, v->value);
						get_next_tok(lex);

						if (lex->cur_tok == CLOSE_CURLY) {
							func_ret_c = 0;
						} else {
							func_ret_c  = 1;
						}
					} else if (strcmp(v->type, "str") == 0) {
						codegen_return_str(cg, v);
						get_next_tok(lex);
					}
				} else {
					printf("ERROR [%d,%d]: variable %s does not exists\n", cur_line, cur_col, lex_id);
					exit(1);
				}
				
			} 
			if (strcmp(func_ret, "void") != 0) {
            	parse_expect(lex, SEMICOLON);
				get_next_tok(lex);
				if (lex->cur_tok == CLOSE_CURLY) {
					func_ret_c = 0;
				} else {
					func_ret_c  = 1;
				}
        	}
		} else {
			if (strcmp(func_ret, "void") != 0) {
				printf("ERROR [%d,%d]: expected return at the end of non void func\n", cur_line, cur_col);
				exit(1);
			} else {
				codegen_return_void(cg);
			}
        }
	
		parse_expect(lex, CLOSE_CURLY);
		codegen_end_function(cg);
	}
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
			return "string";
		default:
			return "unknow token";
	}
}
