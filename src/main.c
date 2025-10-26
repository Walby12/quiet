#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <input_file>\n", argv[0]);
        return 1;
    }
    
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
    CodeGen *cg = codegen_init("output.ssa");
    
    if (!cg) {
        return 1;
    }
    
    parse(lex, cg);
    
    codegen_finish(cg);
    free(content);
    free(lex);
    
    printf("Compiling with QBE...\n");
    int ret = system("qbe output.ssa > output.s");
    if (ret != 0) {
        printf("ERROR: QBE compilation failed\n");
        return 1;
    }
    
    printf("Assembling and linking...\n");
    ret = system("gcc output.s -o program");
    if (ret != 0) {
        printf("ERROR: GCC linking failed\n");
        return 1;
    }
    
    printf("Success! Run with: ./program\n");
    
    return 0;
}
