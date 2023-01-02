#ifndef PARSER
#define PARSER

#include "lexer.h"

void parse_file(Lexer *lexer);

typedef enum {
    UNKNOWN = 0,
    NUMBER = 1,
    BINOP = 2, 
} NodeType;

typedef struct {
    void *node;
    NodeType node_type;
} Node;

typedef struct {
    int value;
} Number;

typedef struct {
    Node left;
    TokenType op;
    Node right;
} BinOp;

#endif