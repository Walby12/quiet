#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

char* read_file(const char* filename) {
    	FILE* f = fopen(filename, "rb");
	if (!f) return NULL;
    	fseek(f, 0, SEEK_END);
    	long size = ftell(f);
    	rewind(f);

    	char* buffer = malloc(size + 1);
    	if (!buffer) { fclose(f); return NULL; }

    	fread(buffer, 1, size, f);
    	buffer[size] = '\0';
    	fclose(f);
    	return buffer;
}

int main() {
    	char* contents = read_file("main.bq");
    	if (!contents) {
        	fprintf(stderr, "Failed to read main.bq\n");
        	return 1;
    	}
	Lexer* lex = lexer_init(contents);
	get_next_tok(lex);
	while (lex->cur_tok != STR_END) {
		get_next_tok(lex);
	}
    	free(contents);
	free(lex);
    	return 0;
}
