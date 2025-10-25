#include "lexer.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

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
	lex->cur_string = NULL;
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
		default:
			printf("%c\n", c);
	}
}
