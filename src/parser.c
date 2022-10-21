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
void decrement_symbol(Token *tokens) {
    token_index--;
    current_token = tokens+token_index;
}

Token *token_peek(Token *tokens) {
    return (tokens+token_index); 
}

NodeReturn return_node(void *node, NodeType node_type) {
    NodeReturn ret;
    ret.node = node;
    ret.node_type = node_type;

    return ret;
}

int var_index = 0;

NodeReturn create_var_reassign_node(char *var_name, NodeReturn expression, int index) {
    VarAssign *result = (VarAssign*)malloc(sizeof(VarAssign));

    result->var_name = var_name;
    result->expression = expression;
    result->index = index;

    return return_node((void *)result, VARASSIGN);
}

NodeReturn create_var_assign_node(char *var_name, NodeReturn expression) {
    VarAssign *result = (VarAssign*)malloc(sizeof(VarAssign));

    result->var_name = var_name;
    result->expression = expression;
    result->index = var_index;

    var_index++; 

    return return_node((void *)result, VARASSIGN);
}

NodeReturn create_bin_op_node(NodeReturn left, Token op, NodeReturn right) {
    BinOp *result = (BinOp*)malloc(sizeof(BinOp));

    result->left = left;
    result->op = op;
    result->right = right;
    return return_node((void *)result, BINOP);
}

NodeReturn create_if_node(NodeReturn expression) {
    IfNode *result = (IfNode*)malloc(sizeof(IfNode));

    result->expression = expression;
    return return_node((void *)result, IF);
}

NodeReturn create_stdout_node(NodeReturn expr) {
    StdOut *result = (StdOut*)malloc(sizeof(StdOut));

    result->expression = expr;
    return return_node((void *)result, STDOUT);
}

NodeReturn create_end_node() {
    End *result = (End*)malloc(sizeof(End));
    //TODO: ADD indexing
    return return_node((void *)result, END);
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
    for (int i = 0; i < variables; i++) {
        if (strcmp(input, var_names[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int getvariable(char *input) {
    for (int i = 0; i < variables; i++) {
        if (strcmp(input, var_names[i]) == 0) {
            return i;
        }
    }
    return 0;
}

int getvariableindex(char *input) {
    for (int i = 0; i < 100; i++) {
        if (strcmp(input, var_names[i]) == 0) {
            return i;
        }
    }
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
        use_var->index = getvariableindex(current_token->value);

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
    //printf("TOK: %s\n", current_token->value);
    //printf("TOK PEEK: %s\n", token_peek(tokens)->value);
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
    else if (strcmp(current_token->value, "if") == 0) {
        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_PARENTHESES) {
            printf("Expected (\n");
            exit(1);
        }

        advance_symbol(tokens);

        NodeReturn if_expression = expression(tokens);
        NodeReturn if_statement = create_if_node(if_expression);

        if (current_token->type != TOK_CLOSE_PARENTHESES) {
            printf("Expected )\n");
            exit(1);
        }

        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_CURLY_BRACE) {
            printf("Expected {\n");
            exit(1);
        }

        return if_statement;
    }   
    else if (strcmp(current_token->value, "}") == 0) {
        NodeReturn res = create_end_node();
        return res;
    }
    else if (strcmp(current_token->value, "stdout") == 0) {
        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_PARENTHESES) {
            printf("Expected (\n");
            exit(1);
        }

        advance_symbol(tokens);

        NodeReturn expr = expression(tokens);
        NodeReturn res = create_stdout_node(expr);

        if (current_token->type != TOK_CLOSE_PARENTHESES) {
            printf("Expected )\n");
            exit(1);
        }

        advance_symbol(tokens);

        if (current_token->type != TOK_SEMI) {
            printf("Expected ;\n");
            exit(1);
        }

        return res;
    }
    else if (isvariable(current_token->value) == 1 && token_peek(tokens)->type == TOK_EQUALS) { //this is different to checking the variable in the term since we need an entire expression
        char *name = current_token->value;
        advance_symbol(tokens);
        //printf("YAY: %s\n", current_token->value);
        if (current_token->type != TOK_EQUALS) {
            printf("Expected =");
            exit(1);
        }

        advance_symbol(tokens);
        NodeReturn var_expression = expression(tokens);
        NodeReturn variable = create_var_reassign_node(name, var_expression, getvariable(name));
        
        if (current_token->type != TOK_SEMI) {
            printf("Expected ;\n");
            exit(1);
        }
        return variable;
    }

    NodeReturn left = term(tokens);

    while (current_token->type == TOK_PLUS || current_token->type == TOK_MINUS || current_token->type == TOK_NOT_EQUAL) {
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
    codegen_var_use(node);
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
    else if (op.type == TOK_NOT_EQUAL) {
        codegen_not_equal(node);
    }

    printf(")");
}

void visit_if_node(NodeReturn node) {
    IfNode *var = (IfNode *)node.node;
    NodeReturn expr = var->expression;
    printf("IF: ");

    visit_node(expr);
    codegen_if(expr);
}

void visit_var_assign_node(NodeReturn node) {
    VarAssign *var = (VarAssign *)node.node;
    NodeReturn expr = var->expression;
    printf("VAR ASSIGN (%s): ", var->var_name);

    visit_node(expr);
    codegen_var(node);
}

void visit_end_node(NodeReturn node) {
    End *end = (End *)node.node;
    codegen_end_node(node);
    printf("END");
}

void visit_stdout_node(NodeReturn node) {
    StdOut *stdout = (StdOut *)node.node;
    NodeReturn expr = stdout->expression;

    printf("STDOUT: ");
    visit_node(expr);
    codegen_stdout(expr);
}

void visit_node(NodeReturn node) {
    if (node.node_type == BINOP) {
        visit_binop(node);
    }
    else if (node.node_type == USEVAR) {
        visit_use_var(node);
    }
    else if (node.node_type == NUMBER) {
        visit_number(node);
    }
    else if (node.node_type == VARASSIGN) {
        visit_var_assign_node(node);
    }
    else if (node.node_type == STDOUT) {
        visit_stdout_node(node);
    }  
    else if (node.node_type == IF) {
        visit_if_node(node);
    }  
    else if (node.node_type == END) {
        visit_end_node(node);
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
    generate_and_visit_node(tokens);
    generate_and_visit_node(tokens);
    generate_and_visit_node(tokens);
    generate_and_visit_node(tokens);
    codegen_end();
}
