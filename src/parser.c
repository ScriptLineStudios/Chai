#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/lexer.h"

int token_index = -1;

enum SymbolTypes {
    LPAREN = 0,
    RPAREN = 1,
    ADD = 2,
    SUB = 3,
    SEMICOLON = 4,
    LET = 5,
    STDOUT = 6,
    EQUALS = 7
};

String current_token;

typedef struct {
    void *left;
    String op;
    void *right;
} BinOp;

typedef struct {
    int value;
} Number;

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

void *factor(String *tokens) {
    Number *number = (Number*)malloc(sizeof(Number));

    if (isnumber(current_token.str) == 1) {
        number->value = atoi(current_token.str);
        advance_symbol(tokens);

        return number;
    }
    return NULL;
}

void *term(String *tokens) {
    void *left = factor(tokens);

    if (current_token.str != NULL) {
        while (strcmp(current_token.str, "*") == 0 || strcmp(current_token.str, "/") == 0) {
            String op = current_token;
            advance_symbol(tokens);

            void *right = factor(tokens);
            left = create_bin_op_node(left, op, right);
        }
    }
    return left;
}

void *expression(String *tokens) {
    void *left = term(tokens);

    while (current_token.str != NULL && (strcmp(current_token.str, "+") == 0 || strcmp(current_token.str, "-") == 0)) {
        String op = current_token;
        advance_symbol(tokens);
        void *right = term(tokens); 
        left = create_bin_op_node(left, op, right);
    }
    return left;
}

void generate_ast(String *tokens) {
    advance_symbol(tokens);
    void *left = expression(tokens);
    BinOp *bin_op = (BinOp *)left; 
    Number *bin_op_left = (Number *)bin_op->left;
    Number *bin_op_right = (Number *)bin_op->right;
    printf("Node left = %d\n", bin_op_left->value);
    printf("Node right = %d\n", bin_op_right->value);

}