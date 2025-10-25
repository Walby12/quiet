#include "codegen.h"
#include "parser.h"

#include <stdlib.h>

CodeGen* codegen_init(const char *filename) {
	CodeGen *cg = malloc(sizeof(CodeGen));
	cg->out = fopen(filename, "w");
	if (!cg->out) {
        	printf("ERROR: Could not open output file %s\n", filename);
		free(cg);
        	return NULL;
    	}
    	cg->temp_counter = 0;
    	cg->label_counter = 0;
    	return cg;
}

void codegen_start_function(CodeGen *cg, const char *name) {
    	fprintf(cg->out, "export function w $%s() {\n", name);
    	fprintf(cg->out, "@start\n");
}

void codegen_putchar(CodeGen *cg, int value) {
    	fprintf(cg->out, "    	%%t%d =w copy %d\n", cg->temp_counter, value);
    	fprintf(cg->out, "    	call $putchar(w %%t%d)\n", cg->temp_counter);
    	cg->temp_counter++;
}

void codegen_return(CodeGen *cg, int value) {
    	fprintf(cg->out, "    	ret %d\n", value);
}

void codegen_variable(CodeGen *cg, Variable *v) {
	fprintf(cg->out, " 	%%%s =w copy %d\n", v->name, v->value);
}

void codegen_end_function(CodeGen *cg) {
    	fprintf(cg->out, "}\n");
}

void codegen_finish(CodeGen *cg) {
    	fclose(cg->out);
    	free(cg);
}
