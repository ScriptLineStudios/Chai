#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

FILE *get_file_ptr();

void codegen_setup();
void codegen_end();

void codegen_number(NodeReturn node, bool is_in_func);
void codegen_add(NodeReturn node, bool is_in_func);
void codegen_mult(NodeReturn node, bool is_in_func);
void codegen_sub(NodeReturn node, bool is_in_func);
void codegen_not_equal(NodeReturn node, bool is_in_func);
void codegen_string_not_equal(NodeReturn node, bool is_in_func);
void codegen_if(NodeReturn node, bool is_in_func);
void codegen_end_node(NodeReturn node, bool is_in_func);
void codegen_while_node(NodeReturn node, bool is_in_func);
void codegen_list(NodeReturn node, bool is_in_func);
void codegen_list_access(NodeReturn node, bool is_in_func);
void codegen_list_reassign(NodeReturn node, bool is_in_func);
void codegen_function(NodeReturn node, bool is_in_func);
void codegen_function_call(NodeReturn node, bool is_in_func);

void codegen_set_parsing_args(bool value);
void codegen_set_arg_num(int value);

void codegen_var(NodeReturn node, bool is_in_func);
void codegen_var_use(NodeReturn node, bool is_in_func);

void codegen_string(NodeReturn node, bool is_in_func);

//Standard Library
void codegen_stdout(NodeReturn node, NodeType type, char **var_types, bool is_in_func);

#endif