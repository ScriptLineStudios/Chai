#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "../include/lexer.h"

int token_index = -1;

typedef enum {
    NULL_TYPE = 0,
    NUMBER = 1,
    BINOP = 2,
    VARASSIGN = 3,
    USEVAR = 4
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
    char *var_name;
    NodeReturn expression;
} VarAssign;

typedef struct {
    char *name;
} UseVar;

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

NodeReturn create_var_assign_node(char *var_name, NodeReturn expression) {
    VarAssign *result = (VarAssign*)malloc(sizeof(VarAssign));

    result->var_name = var_name;
    result->expression = expression;

    return return_node((void *)result, VARASSIGN);
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

char *var_names[100];
int variables = 0;

int isvariable(char *input) {
    for (int i = 0; i < 100; i++) {
        if (strcmp(input, var_names[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

NodeReturn factor(String *tokens) {
    Number *number = (Number*)malloc(sizeof(Number));

    if (isnumber(current_token.str) == 1) {
        number->value = atoi(current_token.str);
        advance_symbol(tokens);

        return return_node(number, NUMBER);
    }
    else if (isvariable(current_token.str) == 1) {
        UseVar *use_var = (UseVar*)malloc(sizeof(UseVar));
        use_var->name = current_token.str;
        advance_symbol(tokens);

        return return_node(use_var, USEVAR);
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
    if (strcmp(current_token.str, "let") == 0) {
        advance_symbol(tokens);
        if (strcmp(current_token.str, "=") == 0) {
            printf("Expected identifier\n");
            exit(1);
        }

        char *var_name = current_token.str;
        var_names[variables] = var_name;
        variables++;
        advance_symbol(tokens);

        if (strcmp(current_token.str, "=") != 0) {
            printf("Expected =\n");
            exit(1);
        }
        advance_symbol(tokens);
        NodeReturn var_expression = expression(tokens);

        NodeReturn variable = create_var_assign_node(var_name, var_expression);

        return variable;
    }

    NodeReturn left = term(tokens);

    while (current_token.str != NULL && (strcmp(current_token.str, "+") == 0 || strcmp(current_token.str, "-") == 0)) {
        String op = current_token;
        advance_symbol(tokens);

        NodeReturn right = term(tokens);

        left = create_bin_op_node(left, op, right);
    }
    return left;
}
 
void visit_number(NodeReturn node) {
    Number *number = (Number *)node.node;
    printf("%d", number->value);
}

void visit_use_var(NodeReturn node) {
    UseVar *use_var = (UseVar *)node.node;
    printf("%s", use_var->name);
}

void visit_binop(NodeReturn node) {
    printf("(");
    BinOp *bin_op = (BinOp *)node.node;    
    NodeReturn left = bin_op->left;
    
    if (left.node_type == BINOP) {
        visit_binop(left);
    }
    else if (left.node_type == NUMBER) {
        visit_number(left);
    }
    else if (left.node_type == USEVAR) {
        visit_use_var(left);
    }

    String op = bin_op->op;
    printf(" %s ", op.str);

    NodeReturn right = bin_op->right;

    if (right.node_type == BINOP) {
        visit_binop(right);
    }
    else if (right.node_type == NUMBER) {
        visit_number(right);
    }
    else if (right.node_type == USEVAR) {
        visit_use_var(right);
    }
    printf(")");
}

void visit_var_assign_node(NodeReturn node) {
    VarAssign *var = (VarAssign *)node.node;
    NodeReturn expr = var->expression;
    printf("VAR ASSIGN (%s): ", var->var_name);

    if (expr.node_type == BINOP) {
        visit_binop(expr);
    }
    else if (expr.node_type == NUMBER) {
        visit_number(expr);
    }
    else if (expr.node_type == USEVAR) {
        visit_use_var(expr);
    }
    else {
        printf("Unknown type: %d\n", expr.node_type);
    } 
}

void generate_and_visit_node(String *tokens) {
    advance_symbol(tokens);
    NodeReturn node = expression(tokens);
    if (node.node_type == BINOP) {
        visit_binop(node);
    }
    else if (node.node_type == NUMBER) {
        visit_number(node);
    }
    else if (node.node_type == VARASSIGN) {
        visit_var_assign_node(node);
    }
    else {
        printf("Unknown type: %d\n", node.node_type);
    }
    printf("\n");
}

void generate_ast(String *tokens) {
    generate_and_visit_node(tokens);
    generate_and_visit_node(tokens);
}