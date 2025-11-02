quiet:
	gcc -Isrc -o quiet src/parser.c src/lexer.c src/main.c src/codegen.c -Wall -Wextra
clean:
	rm -f quiet
