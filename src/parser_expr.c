#include "parser.h"
#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_primary(Lexer *lex, CodeGen *cg) {
    if (lex->cur_tok == NUM) {
        int value = lex_num;
        get_next_tok(lex);
        return value;
    } else if (lex->cur_tok == OPEN_PAREN) {
        get_next_tok(lex);
        int value = parse_expression(lex, cg);
        if (lex->cur_tok != CLOSE_PAREN) {
            printf("ERROR [%d,%d]: expected closing parenthesis\n", cur_line, cur_col);
            exit(1);
        }
        get_next_tok(lex);
        return value;
    } else if (lex->cur_tok == ID) {
        if (variable_exists(lex_id)) {
            Variable *v = get_variable(lex_id);
            if (strcmp(v->type, "int") != 0) {
                printf("ERROR [%d,%d]: variable %s must be int for arithmetic\n", cur_line, cur_col, lex_id);
                exit(1);
            }
            int value = v->value;
            get_next_tok(lex);
            return value;
        } else {
            printf("ERROR [%d,%d]: unknown variable: %s\n", cur_line, cur_col, lex_id);
            exit(1);
        }
    } else {
        printf("ERROR [%d,%d]: unexpected token in expression: %s\n", cur_line, cur_col, to_string(lex->cur_tok));
        exit(1);
    }
    return 0;
}

// Factor handles multiplication and division
int parse_factor(Lexer *lex, CodeGen *cg) {
    int result = parse_primary(lex, cg);
    
    while (lex->cur_tok == MULTIPLY || lex->cur_tok == DIVIDE) {
        TokenType op = lex->cur_tok;
        get_next_tok(lex);
        int rhs = parse_primary(lex, cg);
        
        if (op == MULTIPLY) {
            result *= rhs;
        } else {
            if (rhs == 0) {
                printf("ERROR [%d,%d]: division by zero\n", cur_line, cur_col);
                exit(1);
            }
            result /= rhs;
        }
    }
    
    return result;
}

// Term handles addition and subtraction
int parse_term(Lexer *lex, CodeGen *cg) {
    int result = parse_factor(lex, cg);
    
    while (lex->cur_tok == PLUS || lex->cur_tok == MINUS) {
        TokenType op = lex->cur_tok;
        get_next_tok(lex);
        int rhs = parse_factor(lex, cg);
        
        if (op == PLUS) {
            result += rhs;
        } else {
            result -= rhs;
        }
    }
    
    return result;
}

// Expression is the top-level entry point for parsing expressions
int parse_expression(Lexer *lex, CodeGen *cg) {
    return parse_term(lex, cg);
}