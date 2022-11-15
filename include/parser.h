#pragma once
#include "lexer.h"
#include <stdbool.h>

typedef enum {
    NULL_TYPE = 0,
    NUMBER = 1,
    BINOP = 2,
    VARASSIGN = 3,
    USEVAR = 4,
    STDOUT = 5,
    IF = 6,
    END = 7,
    STRING = 8,
    WHILE = 9,
    LIST = 10,
    LISTACCESS = 11,
    LISTREASSIGN = 12,
    FUNCTION = 13,
    FUNCTIONCALL = 14,
} NodeType;

//be hold the list of nodes

typedef struct {
    void *node;
    NodeType node_type;
    int position;
} NodeReturn;

typedef struct {    
    char **values;
    int size;
} List;

typedef struct {
    char **args;
    int arg_size;
    NodeReturn *expressions;
    int num_expressions;
} Function;

typedef struct {
    char *function_name;
    int num_args;
    NodeReturn args;
} FunctionCall;

typedef struct {
    NodeReturn left;
    Token op;
    NodeReturn right;
    int stack_pos;
} BinOp;

typedef struct {
    char *value;
} String;

typedef struct {
    char *var_name;
    int index;
    NodeReturn expression;
} VarAssign;

typedef struct {
    char *name;
    int index;
} UseVar;

typedef struct {
    char *name;
    int index;
    NodeReturn expression;
} ListAccess;

typedef struct {
    char *name;
    int index;
    NodeReturn expression;
    NodeReturn assignment;
} ListReassign;

typedef struct {
    NodeReturn expression;
} StdOut;

typedef struct {
    NodeReturn expression;
    int stack_pos;
} IfNode;

typedef struct {
    NodeReturn expression;
    int stack_pos;
} WhileNode;

typedef struct {
    int stack_pos;
    NodeType ending;
} End;

typedef struct {
    int value;
} Number;

void generate_ast(Token *tokens, int ntokens);
void visit_node(NodeReturn node, bool is_in_func);
int getvariableindex(char *input);
int getfunctionarg(char *input);
NodeType visit_node_and_get_type(NodeReturn node, bool is_in_func);

