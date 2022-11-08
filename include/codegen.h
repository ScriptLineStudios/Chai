#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

void codegen_setup();
void codegen_end();

void codegen_number(NodeReturn node);
void codegen_add(NodeReturn node);
void codegen_mult(NodeReturn node);
void codegen_not_equal(NodeReturn node);
void codegen_string_not_equal(NodeReturn node);
void codegen_if(NodeReturn node);
void codegen_end_node(NodeReturn node);
void codegen_while_node(NodeReturn node);

void codegen_var(NodeReturn node);
void codegen_var_use(NodeReturn node);

void codegen_string(NodeReturn node);

//Standard Library
void codegen_stdout(NodeReturn node);

#endif