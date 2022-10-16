#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/lexer.h"

int token_index = -1;

typedef enum {
    NULL_TYPE = 0,
    NUMBER = 1,
    BINOP = 2
} NodeType;

String current_token;

typedef struct {
    void *left;
    String op;
    void *right;
} BinOp;

typedef struct {
    int value;
} Number;

typedef struct {
    void *node;
    NodeType node_type;
} NodeReturn;

void advance_symbol(String *tokens) {
    token_index += 1;
    current_token = tokens[token_index];
}

void *create_bin_op_node(void *left, String op, void *right) {
    BinOp *result = (BinOp*)malloc(sizeof(BinOp));

    result->left = left;
    result->op = op;
    result->right = right;
    return result;
}

int isnumber(char *input) {
    int length = strlen(input);
    for (int i=0;i<length; i++)
    if (!isdigit(input[i])) {
        return 0;
    }
    return 1;
}

NodeReturn return_node(void *node, NodeType node_type) {
    NodeReturn ret;
    ret.node = node;
    ret.node_type = node_type;

    return ret;
}

NodeReturn factor(String *tokens) {
    Number *number = (Number*)malloc(sizeof(Number));

    if (isnumber(current_token.str) == 1) {
        number->value = atoi(current_token.str);
        advance_symbol(tokens);

        return return_node(number, NUMBER);
    }
    return return_node(NULL, NULL_TYPE);
}

NodeReturn term(String *tokens) {
    NodeReturn _left = factor(tokens);
    void *left = _left.node;

    while (current_token.str != NULL && (strcmp(current_token.str, "*") == 0 || strcmp(current_token.str, "/") == 0)) {
        String op = current_token;
        advance_symbol(tokens);

        NodeReturn _right = factor(tokens);
        void *right = _right.node;
        
        left = create_bin_op_node(left, op, right);
    }
    return return_node(left, BINOP);
}

NodeReturn expression(String *tokens) {
    NodeReturn _left = factor(tokens);
    void *left = _left.node;

    while (current_token.str != NULL && (strcmp(current_token.str, "+") == 0 || strcmp(current_token.str, "-") == 0)) {
        String op = current_token;
        advance_symbol(tokens);

        NodeReturn _right = factor(tokens);
        void *right = _right.node;

        left = create_bin_op_node(left, op, right);
    }
    return return_node(left, BINOP);
}

void generate_ast(String *tokens) {
    advance_symbol(tokens);
    NodeReturn node = expression(tokens);
    BinOp *bin_op = node.node;

    Number *left = (Number *)bin_op->left;
    Number *right = (Number *)bin_op->right;

    printf("%d\n", left->value);
    printf("%d\n", right->value);   
}