#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

void codegen_setup();
void codegen_end();

void codegen_number(NodeReturn node);
void codegen_add(NodeReturn node);
void codegen_mult(NodeReturn node);

void codegen_var(NodeReturn node);
void codegen_var_use(NodeReturn node);

//Standard Library
void codegen_stdout(NodeReturn node);

#endif