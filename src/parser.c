#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "lexer.h"
#include "codegen.h"

int token_index = 0;

Token *current_token;

void advance_symbol(Token *tokens) {
    current_token = tokens+token_index;
    token_index++;
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

NodeReturn create_bin_op_node(NodeReturn left, Token op, NodeReturn right) {
    BinOp *result = (BinOp*)malloc(sizeof(BinOp));

    result->left = left;
    result->op = op;
    result->right = right;
    return return_node((void *)result, BINOP);
}

int isnumber(char *input) {
    int length = strlen(input);
    for (int i = 0; i < length; i++) {
        if (!isdigit(input[i])) {
            return 0;
        }    
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

NodeReturn factor(Token *tokens) {
    Number *number = (Number*)malloc(sizeof(Number));

    if (current_token->type == TOK_NUMBER) {
        number->value = atoi(current_token->value);
        advance_symbol(tokens);

        return return_node(number, NUMBER);
    }

    else if (isvariable(current_token->value) == 1) {
        UseVar *use_var = (UseVar*)malloc(sizeof(UseVar));
        use_var->name = current_token->value;
        advance_symbol(tokens);

        return return_node(use_var, USEVAR);
    }
    return return_node(NULL, NULL_TYPE);
}

NodeReturn term(Token *tokens) {
    NodeReturn left = factor(tokens);

    while (current_token->type == TOK_MULT || current_token->type == TOK_DIV) {
        Token op = *current_token;
        advance_symbol(tokens);

        NodeReturn right = factor(tokens);

        left = create_bin_op_node(left, op, right);
    }
    return left;
}

NodeReturn expression(Token *tokens) {
    if (strcmp(current_token->value, "let") == 0) {
        advance_symbol(tokens);
        if (current_token->type != TOK_IDENT) {
            printf("Expected identifier\n");
            exit(1);
        }

        char *var_name = current_token->value;
        var_names[variables++] = var_name;
        advance_symbol(tokens);

        if (current_token->type != TOK_EQUALS) {
            printf("Expected =\n");
            exit(1);
        }
        advance_symbol(tokens);
        NodeReturn var_expression = expression(tokens);

        NodeReturn variable = create_var_assign_node(var_name, var_expression);

        return variable;
    }

    NodeReturn left = term(tokens);

    while (current_token->type == TOK_PLUS || current_token->type == TOK_MINUS) {
        Token op = *current_token;
        advance_symbol(tokens);

        NodeReturn right = term(tokens);

        left = create_bin_op_node(left, op, right);
    }
    return left;
}

void visit_node(NodeReturn node);
 
void visit_number(NodeReturn node) {
    Number *number = (Number *)node.node;
    codegen_number(node);
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
    
    visit_node(left);

    Token op = bin_op->op;
    printf(" %s ", op.value);

    NodeReturn right = bin_op->right;

    visit_node(right);
    if (op.type == TOK_PLUS) {
        codegen_add(node);
    }
    else if (op.type == TOK_MULT) {
        codegen_mult(node);
    }

    printf(")");
}


void visit_var_assign_node(NodeReturn node) {
    VarAssign *var = (VarAssign *)node.node;
    NodeReturn expr = var->expression;
    printf("VAR ASSIGN (%s): ", var->var_name);

    visit_node(expr);
    codegen_var(expr);
}

void visit_node(NodeReturn node) {
    if (node.node_type == BINOP) {
        visit_binop(node);
    }
    else if (node.node_type == NUMBER) {
        visit_number(node);
    }
    else if (node.node_type == VARASSIGN) {
        visit_var_assign_node(node);
    }
    else if (node.node_type == USEVAR) {
        visit_use_var(node);
    }
    else {
        printf("Unknown type: %d\n", node.node_type);
    }
}

void generate_and_visit_node(Token *tokens) {
    advance_symbol(tokens);
    NodeReturn node = expression(tokens);

    visit_node(node);
    printf("\n");
}

void generate_ast(Token *tokens, int ntokens) {
    codegen_setup();
    generate_and_visit_node(tokens);
    codegen_end();
}
