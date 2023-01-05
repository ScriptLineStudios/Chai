#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#include "../include/parser.h"
#include "../include/lexer.h"
#include "../include/error.h" 

void visit_node(Node node);
#define ADVANCE advance(lexer)
#define CAST(type, name) type *name = (type *)node.node;
#define TOK_MATCH(T, error) do{advance(lexer);if(current_token.type!=T){raise_compile_error(&current_token,error);}}while(false) //very readable
#define STATEMENT_MATCH(name) strcmp(current_token.value, name) == 0

Token current_token;

void advance(Lexer *lexer) {
    current_token = get_next_token(lexer);
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

    if (current_token.type == TOK_NUMBER) {
        number->value = atoi(current_token.value);
        ADVANCE;

        return return_node((void *)number, NUMBER);
    }
}

Node term(Lexer *lexer) {
    Node left = factor(lexer);

    while (current_token.type == TOK_MULT || current_token.type == TOK_DIV) {
        Token op = current_token;
        ADVANCE;
        Node right = factor(lexer);

        left = create_bin_op_node(left, op, right);
    }

    return left;
}

Node expression(Lexer *lexer) {    
    ADVANCE;
    printf("%d\n", current_token.type);
    Node left = term(lexer);
    while (current_token.type == TOK_ADD || current_token.type == TOK_SUB) {
        Token op = current_token;
        ADVANCE;
        Node right = term(lexer);

        left = create_bin_op_node(left, op, right);
    }

    return left;
}

Node get_node(Lexer *lexer) {
    ADVANCE;
    if (STATEMENT_MATCH("let")) {
        printf("Found var decleration!\n");
        TOK_MATCH(TOK_IDENTIFIER, "Expected Identifier!");
        printf("Variable name = %s\n", current_token.value);
    }       
}


void visit_binop(Node node) {
    CAST(BinOp, bin_op);
    
    printf("(");
    Node left = bin_op->left;
    visit_node(left);
    
    TokenType op = bin_op->op;

    switch (op) {
        case TOK_ADD:
            printf(" + ");
            break;
        case TOK_DIV:
            printf(" / ");
            break;
        case TOK_MULT:
            printf(" * ");
            break;
        case TOK_SUB:
            printf(" - "); 

    }

    Node right = bin_op->right;
    visit_node(right);
    printf(")"); 
}

void visit_number(Node node) {
    CAST(Number, number);

    printf("%d", number->value);
}

void visit_node(Node node) {
    switch (node.node_type) {
        case BINOP:
            visit_binop(node);
            break;
        case NUMBER: 
            visit_number(node);
            break;
    }
}

void create_node(Lexer *lexer) {
    Node node = get_node(lexer);
    visit_node(node);
    printf("\n");
}

void parse_file(Lexer *lexer) {
    create_node(lexer); // for now we shall focus on compiling only one node :D
}