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
} NodeType;

typedef struct {
    void *node;
    NodeType node_type;
} NodeReturn;

typedef struct {
    NodeReturn left;
    Token op;
    NodeReturn right;
} BinOp;

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
    NodeReturn expression;
} StdOut;

typedef struct {
    NodeReturn expression;
} IfNode;

typedef struct {
    int value;
} Number;

void generate_ast(Token *tokens, int ntokens);
