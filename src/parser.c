#include "stdio.h"
#include "stdlib.h"

#include "../include/parser.h"
#include "../include/lexer.h"
 
#define ADVANCE advance(lexer)

Token *current_token;

void advance(Lexer *lexer) {
    Token temp = get_next_token(lexer);
    current_token = &temp;
}   

Node return_node(void *node, NodeType node_type) {
    Node ret;
    ret.node = node;
    ret.node_type = node_type;

    return ret;
}

Node create_bin_op_node(Node left, Token op, Node right) {
    BinOp *bin_op = malloc(sizeof(BinOp));
    bin_op->left = left;
    bin_op->op = op.type;
    bin_op->right = right;

    return return_node((void *)bin_op, BINOP);
}

Node factor(Lexer *lexer) {
    Number *number = malloc(sizeof(Number));

    if (current_token->type == TOK_NUMBER) {
        number->value = atoi(current_token->value);
        ADVANCE;

        return return_node((void *)number, NUMBER);
    }
}

Node term(Lexer *lexer) {
    Node left = factor(lexer);

    while (current_token->type == TOK_MULT || current_token->type == TOK_DIV) {
        Token op = *current_token;
        ADVANCE;
        Node right = factor(lexer);

        left = create_bin_op_node(left, op, right);
    }

    return left;
}

Node expression(Lexer *lexer) {    
    ADVANCE;

    Node left = term(lexer);
    while (current_token->type == TOK_ADD || current_token->type == TOK_SUB) {
        Token op = *current_token;
        ADVANCE;
        Node right = term(lexer);

        left = create_bin_op_node(left, op, right);
    }

    return left;
}

void create_node(Lexer *lexer) {
    Node node = expression(lexer);
    printf("%d\n", node.node_type);
}

void parse_file(Lexer *lexer) {
    create_node(lexer); // for now we shall focus on compiling only one node :D
}