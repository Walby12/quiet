#include "lexer.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

char lex_id[1024];
char lex_str[1024];
char lex_format[256];
int lex_format_index = 0;
int lex_num;
int cur_line = 1;
int cur_col = 0;

Lexer* lexer_init(const char *cont) {
	Lexer *lex = malloc(sizeof(Lexer));
    if (lex == NULL) {
        printf("Could not allocate memory\n");
        return NULL;
    }
    lex->cont = cont;
    lex->index = 0;
    return lex;
}

void get_next_tok(Lexer *lex) {
	while (1) {
		char c = lex->cont[lex->index];
		if (c == ' ' || c == '\t') {
			lex->index++;
			cur_col++;
		} else if (c == '\n') {
			lex->index++;
			cur_col = 0;
			cur_line++;
		} else {
			break;
		}
	}
	
	char c = lex->cont[lex->index++];
	cur_col++;
	
	switch (c) {
		case '\0':
			lex->cur_tok = STR_END;
			lex->index--;
			break;
		case '(':
			lex->cur_tok = OPEN_PAREN;
			break;
		case ')':
			lex->cur_tok = CLOSE_PAREN;
			break;
		case ';':
			lex->cur_tok = SEMICOLON;
			break;
		case '{':
			lex->cur_tok = OPEN_CURLY;
			break;
		case '}':
			lex->cur_tok = CLOSE_CURLY;
			break;
		case ',':
			lex->cur_tok = COMMA;
			break;
		case '-':
			if (lex->cont[lex->index] != '>') {
				printf("ERROR [%d,%d]: unknown token: %c\n", cur_line, cur_col, c);
				exit(1);
			} else {
				lex->cur_tok = ARROW;
				cur_col++;
				lex->index++;
				break;
			}
		case '=':
			lex->cur_tok = EQUALS;
			break;
		case '\"':
			char builder[1024];
			int i = 0;

			while (lex->cont[lex->index] != '\0' && lex->cont[lex->index] != '\"' && i < (int) sizeof(builder) - 1) {
				if (builder[i] == '%') {
					printf("Noice");
				}
				builder[i++] = lex->cont[lex->index++];
				cur_col++;
			}
    		if (lex->cont[lex->index] == '\"') {
        		lex->index++; 
        		cur_col++;
    		} else {
				printf("ERROR [%d,%d]: unclosed string: %s", cur_line, cur_col, builder);
				exit(1);
    		}

    		builder[i] = '\0';
			memcpy(lex_str, builder, i + 1);
    		lex->cur_tok = STRING;
			for (int j = 0; j < (int) strlen(lex_str); ++j) {
				if (lex_str[j] == '%') {
					switch (lex_str[++j]) {
						case 's':
							lex->cur_tok = STRING_FORMAT;
							lex_format[lex_format_index++] = 's';
							break;
						case 'd':
							lex->cur_tok = STRING_FORMAT;
							lex_format[lex_format_index++] = 'd';
							break;
						default:
							printf("ERROR [%d,%d]: unknow string format: %%%c\n", cur_line, cur_col, lex_str[j]);
							exit(1);
					}
				}
			}
    		break;
		default:
			if (isalpha(c)) {
				char builder[1024];
				int i = 0;
				builder[i++] = c;
				
				while (lex->cont[lex->index] != '\0' && isalpha(lex->cont[lex->index]) && i < (int) sizeof(builder) - 1) {
					builder[i++] = lex->cont[lex->index++];
					cur_col++;
				}
				builder[i] = '\0';
				memcpy(lex_id, builder, i + 1);
				
				if (strcmp(lex_id, "fn") == 0) {
					lex->cur_tok = FN;
				} else if (strcmp(lex_id, "return") == 0) {
					lex->cur_tok = RETURN;
				} else {
					lex->cur_tok = ID;
				}
			} else if (isdigit(c)) {
				char builder[1024];
				int i = 0;
				builder[i++] = c;
				
				while (isdigit(lex->cont[lex->index]) && 
				    lex->cont[lex->index] != '\0' && 
				    i < (int) sizeof(builder) - 1) {
					builder[i++] = lex->cont[lex->index++];
					cur_col++;
				}
				builder[i] = '\0';
				lex_num = atoi(builder);
				lex->cur_tok = NUM;
			} else {
				printf("ERROR [%d,%d]: unknown token: %c\n", cur_line, cur_col, c);
				exit(1);
			}
	}
}
