#include "lexer.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

char lex_id[1024];
int lex_num;
int cur_line = 1;
int cur_col = 0;

Lexer* lexer_init(const char *cont) {
	Lexer *lex = malloc(sizeof(Lexer) + 1);
	if (lex == NULL) {
		printf("Could not allocate memory\n");
		return NULL;
	}
	lex->cont = cont;
	lex->index = 0;
	return lex;
}

void get_next_tok(Lexer *lex) {
	char c = lex->cont[lex->index++];
	switch (c) {
		case ' ':
			cur_col++;
			break;
		case '\n':
			cur_col = 0;
			cur_line++;
			break;
		case '\t':
			cur_col++;
			break;
		case '\0':
			lex->cur_tok = STR_END;
			break;
		case '(':
			lex->cur_tok = OPEN_PAREN;
			cur_col++;
			break;
		case ')':
			lex->cur_tok = CLOSE_PAREN;
			cur_col++;
			break;
		case ';':
			lex->cur_tok = SEMICOLON;
			cur_col++;
			break;
		case '{':
			lex->cur_tok = OPEN_CURLY;
			cur_col++;
			break;
		case '}':
			lex->cur_tok = CLOSE_CURLY;
			cur_col++;
			break;
		case '-':
			if (lex->cont[lex->index] != '>') {
				printf("ERROR [%d,%d]: unknow token: %c\n", cur_col, cur_line, c);
				exit(1);
			} else {
				lex->cur_tok = ARROW;
				cur_col++;
				lex->index++;
				break;
			}
		default:
			if (isalpha(c)) {
				char builder[1024];
				int i = 0;
				while (isalpha(c) && c != '\0' && i < (int) sizeof(builder) - 1) {
					builder[i++] = c;
					c = lex->cont[lex->index++];
					cur_col++;
				}
				builder[i] = '\0';
				memcpy(lex_id, builder, i);
				lex_id[i] = '\0';
				lex->cur_tok = ID;
			} else if (isdigit(c)) {
				char builder[1024];
				int i = 0;
				while (isdigit(c) && c != '\0' && i < (int) sizeof(builder) - 1) {
					builder[i++] = c;
					c = lex->cont[lex->index++];
					cur_col++;
				}
				builder[i] = '\0';
				lex_num = atoi(builder);
				lex->cur_tok = NUM;
			} else {
				printf("ERROR [%d,%d]: unknow token: %c\n", cur_col, cur_line, c);
				exit(1);
			}
	}
}
