#pragma once
#include "lexer.h"

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
void visit_node(NodeReturn node);
NodeType visit_node_and_get_type(NodeReturn node);

