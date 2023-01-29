#ifndef CODEGEN
#define CODEGEN
#include "../include/parser.h"

void codegen_init();
void codegen_end();

void codegen_set_parsing_if(int index);

void queue_string_gen(char *string);

void codegen_number(Node node);
void codegen_binop(Node node);
void codegen_var_decal(Node node);
void codegen_print(Node node);
void codegen_var_access(Node node);
void codegen_if(Node node);
void codegen_string(Node node);

#endif