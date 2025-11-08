#ifndef LEXER_H
#define LEXER_H

extern char lex_id[1024];
extern char lex_str[1024];
extern char lex_format[256];
extern int lex_format_index;
extern int lex_num;
extern int cur_line;
extern int cur_col;

typedef enum {
	ID,
	NUM,
	OPEN_PAREN,
	CLOSE_PAREN,
	SEMICOLON,
	OPEN_CURLY,
	CLOSE_CURLY,
	ARROW,
	FN,
	RETURN,
	EQUALS,
	STRING,
	STRING_FORMAT,
	COMMA,
	STR_END
} TokenType;

typedef struct {
	const char *cont;
	int index;
	TokenType cur_tok;
} Lexer;

Lexer* lexer_init(const char *cont);

void get_next_tok(Lexer *lex);

#endif
