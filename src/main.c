#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

void strip_ext(char *fname) {
	char *end = fname + strlen(fname);

	while (end > fname && *end != '.') {
		--end;
    }

    if (end > fname) {
    	*end = '\0';
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

	char file_name[256];
	strcpy(file_name, argv[1]);
	strip_ext(file_name);
    
    FILE *f = fopen(argv[1], "r");
    if (!f) {
        printf("ERROR: Could not open file %s\n", argv[1]);
        return 1;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    
    Lexer *lex = lexer_init(content);
	
	char ssa_name[300];
	snprintf(ssa_name, sizeof(ssa_name), "%s.ssa", file_name);
    CodeGen *cg = codegen_init(ssa_name);
    
    if (!cg) {
        return 1;
    }
    
    parse(lex, cg);
    
    codegen_finish(cg);
    free(content);
    free(lex);
    
	char command1[600];
	snprintf(command1, sizeof(command1), "qbe %s.ssa > %s.s", file_name, file_name);

	printf("Compiling with QBE...\n");
    int ret = system(command1);
    if (ret != 0) {
        printf("ERROR: QBE compilation failed\n");
        return 1;
    }
   	
	char command2[600];
	snprintf(command2, sizeof(command2), "gcc %s.s -o %s", file_name, file_name);

    printf("Assembling and linking...\n");
    ret = system(command2);
    if (ret != 0) {
        printf("ERROR: GCC linking failed\n");
        return 1;
    }
    
    printf("Success! Run with: ./%s\n", file_name);
 
    return 0;
}
