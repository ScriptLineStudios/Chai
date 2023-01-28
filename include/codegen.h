#ifndef CODEGEN
#define CODEGEN
#include "../include/parser.h"

void codegen_init();
void codegen_end();

void codegen_number(Node node);
void codegen_binop(Node node);

#endif