typedef enum {
	ID,
	STR_END
} TokenType;

typedef struct {
	const char *cont;
	char *cur_string;
	int index;
	TokenType cur_tok;
} Lexer;

Lexer* lexer_init(const char *cont);

void get_next_tok(Lexer *lex);
