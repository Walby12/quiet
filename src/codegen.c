#include "codegen.h"
#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int str_index = 0;
String *head = NULL;

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
    fprintf(cg->out, "    %%t%d =w copy %d\n", cg->temp_counter, value);
    fprintf(cg->out, "    call $putchar(w %%t%d)\n", cg->temp_counter);
    cg->temp_counter++;
}

void codegen_putchar_variable(CodeGen *cg, const char *value) {
    fprintf(cg->out, "    call $putchar(w %%%s)\n", value);
}

void codegen_return(CodeGen *cg, int value) {
    fprintf(cg->out, "    ret %d\n", value);
}

void codegen_variable_int(CodeGen *cg, Variable *v) {
    fprintf(cg->out, "    %%%s =w copy %d\n", v->name, v->value);
}

void codegen_variable_str(CodeGen *cg, Variable *v) {
    fprintf(cg->out, "    %%%s =l copy $%s\n", v->name, v->data_label);
}

void codegen_variable_reassign_int(CodeGen *cg, Variable *v, int value) {
    fprintf(cg->out, "    %%%s =w copy %d\n", v->name, value);
}

void codegen_variable_reassign_str(CodeGen *cg, Variable *v) {
    fprintf(cg->out, "    %%%s =l copy $%s\n", v->name, v->data_label);
}

void codegen_end_function(CodeGen *cg) {
    fprintf(cg->out, "}\n");
}

void codegen_emit_strings(CodeGen *cg) {
    String *current = head;
    if (current == NULL) {
        return;
    }

    while (current != NULL) {
        fprintf(cg->out, "data $%s = { b \"%s\", b 0 }\n", current->label, current->value);
        current = current->next;
    }
}

void codegen_finish(CodeGen *cg) {
    codegen_emit_strings(cg);
    String *current = head;
    String *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    head = NULL;

    fclose(cg->out);
    free(cg);
}

void append_string(Variable *v, const char *value) {
    String *new_str = (String *)malloc(sizeof(String));
    if (new_str == NULL) {
        perror("Failed to allocate memory");
        return;
    }

    new_str->v = v;

    snprintf(new_str->label, sizeof(new_str->label), "%s_%d", v->name, str_index++);
    strncpy(v->data_label, new_str->label, sizeof(v->data_label));
    v->data_label[sizeof(v->data_label) - 1] = '\0';

    strncpy(new_str->value, value, sizeof(new_str->value));
    new_str->value[sizeof(new_str->value) - 1] = '\0';
    new_str->next = NULL;

    if (head == NULL) {
        head = new_str;
        return;
    }

    String *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_str;
}

void print_strings() {
    String *current = head;
    printf("--- String List ---\n");
    while (current != NULL) {
        printf("Label: %s, Variable name: %s, Value: %s\n",
               current->label,
               current->v->name,
               current->value);
        current = current->next;
    }
}
