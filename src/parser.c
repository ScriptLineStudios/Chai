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
    void *node;
    NodeType node_type;
} NodeReturn;

typedef struct {
    NodeReturn left;
    String op;
    NodeReturn right;
} BinOp;

typedef struct {
    int value;
} Number;

void advance_symbol(String *tokens) {
    token_index += 1;
    current_token = tokens[token_index];
}

NodeReturn return_node(void *node, NodeType node_type) {
    NodeReturn ret;
    ret.node = node;
    ret.node_type = node_type;

    return ret;
}

NodeReturn create_bin_op_node(NodeReturn left, String op, NodeReturn right) {
    BinOp *result = (BinOp*)malloc(sizeof(BinOp));

    result->left = left;
    result->op = op;
    result->right = right;
    return return_node((void *)result, BINOP);
}

int isnumber(char *input) {
    int length = strlen(input);
    for (int i=0;i<length; i++)
    if (!isdigit(input[i])) {
        return 0;
    }
    return 1;
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
    NodeReturn left = factor(tokens);

    while (current_token.str != NULL && (strcmp(current_token.str, "*") == 0 || strcmp(current_token.str, "/") == 0)) {
        String op = current_token;
        advance_symbol(tokens);

        NodeReturn right = factor(tokens);

        left = create_bin_op_node(left, op, right);
    }
    return left;
}

NodeReturn expression(String *tokens) {
    NodeReturn left = term(tokens);

    while (current_token.str != NULL && (strcmp(current_token.str, "+") == 0 || strcmp(current_token.str, "-") == 0)) {
        String op = current_token;
        advance_symbol(tokens);

        NodeReturn right = term(tokens);

        left = create_bin_op_node(left, op, right);
    }
    return left;
}
 
void vist_number(NodeReturn node) {
    Number *number = (Number *)node.node;
    printf("%d", number->value);
}

void vist_binop(NodeReturn node) {
    printf("(");
    BinOp *bin_op = (BinOp *)node.node;    
    NodeReturn left = bin_op->left;
    
    if (left.node_type == BINOP) {
        vist_binop(left);
    }
    else if (left.node_type == NUMBER) {
        vist_number(left);
    }

    String op = bin_op->op;
    printf("%s", op.str);

    NodeReturn right = bin_op->right;

    if (right.node_type == BINOP) {
        vist_binop(right);
    }
    else if (right.node_type == NUMBER) {
        vist_number(right);
    }
    printf(")");
}

void generate_ast(String *tokens) {
    advance_symbol(tokens);
    NodeReturn node = expression(tokens);

    if (node.node_type == BINOP) {
        vist_binop(node);
    }
    else if (node.node_type == NUMBER) {
        vist_number(node);
    }
    printf("\n");
}