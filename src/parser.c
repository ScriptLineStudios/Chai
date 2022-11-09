#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>

#include "lexer.h"
#include "codegen.h"

int token_index = 0;
int if_statement_stack = -1;

Token *current_token;

int stack_pointer;
int *branch_stack;

int if_stack_pointer;
int *if_stack;

int while_stack_pointer;
int *while_stack;

void setup_stack() {
    stack_pointer = 0;
    branch_stack = malloc(sizeof(int) * stack_pointer);

    if_stack_pointer = 0;
    if_stack = malloc(sizeof(int) * if_stack_pointer); //TODO: FINISH THIS STACK!!!

    while_stack_pointer = 0;
    while_stack = malloc(sizeof(int) * while_stack_pointer);
}

void PUSH(int x) {
    stack_pointer++;
    branch_stack = realloc(branch_stack, sizeof(int) * stack_pointer);
    branch_stack[stack_pointer] = x;
}

int POP() {
    int ret = branch_stack[stack_pointer];
    stack_pointer--;
    branch_stack = realloc(branch_stack, sizeof(int) * stack_pointer);
    return ret;
}

int PEEK() {
    return branch_stack[stack_pointer];
}

void PUSHIF(int x) {
    if_stack_pointer++;
    if_stack = realloc(if_stack, sizeof(int) * if_stack_pointer);
    if_stack[if_stack_pointer] = x;
}

int POPIF() {
    int ret = if_stack[if_stack_pointer];
    if_stack_pointer--;
    if_stack = realloc(if_stack, sizeof(int) * if_stack_pointer);
    return ret;
}

int PEEKIF() {
    return if_stack[if_stack_pointer];
}

void PUSHWHILE(int x) {
    while_stack_pointer++;
    while_stack = realloc(while_stack, sizeof(int) * while_stack_pointer);
    while_stack[while_stack_pointer] = x;
}

int POPWHILE() {
    int ret = while_stack[while_stack_pointer];
    while_stack_pointer--;
    while_stack = realloc(while_stack, sizeof(int) * while_stack_pointer);
    return ret;
}

int PEEKWHILE() {
    return while_stack[while_stack_pointer];
}

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
    ret.position = current_token->position;

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

NodeReturn create_bin_op_node(NodeReturn left, Token op, NodeReturn right, int stack_pos) {
    BinOp *result = (BinOp*)malloc(sizeof(BinOp));

    result->left = left;
    result->op = op;
    result->right = right;
    result->stack_pos = stack_pos;

    return return_node((void *)result, BINOP);
}

NodeReturn create_while_node(NodeReturn expression, int stack_pos) {
    WhileNode *result = (WhileNode*)malloc(sizeof(WhileNode));

    result->expression = expression;
    result->stack_pos = stack_pos;
    return return_node((void *)result, WHILE);
}

NodeReturn create_if_node(NodeReturn expression, int stack_pos) {
    IfNode *result = (IfNode*)malloc(sizeof(IfNode));

    result->expression = expression;
    result->stack_pos = stack_pos;
    return return_node((void *)result, IF);
}

NodeReturn create_stdout_node(NodeReturn expr) {
    StdOut *result = (StdOut*)malloc(sizeof(StdOut));

    result->expression = expr;
    return return_node((void *)result, STDOUT);
}

NodeReturn create_end_node(int stack_pos, NodeType ending) {
    End *result = (End*)malloc(sizeof(End));
    result->stack_pos = stack_pos;
    result->ending = ending;
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
char *var_types[100];
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

    else if (current_token->type == TOK_STRING) {
        String *string = malloc(sizeof(String));

        string->value = current_token->value;
        advance_symbol(tokens);

        return return_node(string, STRING);
    }
    return return_node(NULL, NULL_TYPE);
}

NodeReturn term(Token *tokens) {
    NodeReturn left = factor(tokens);

    while (current_token->type == TOK_MULT || current_token->type == TOK_DIV) {
        Token op = *current_token;
        advance_symbol(tokens);

        NodeReturn right = factor(tokens);

        left = create_bin_op_node(left, op, right, PEEK());
    }
    return left;
}

bool parsingIf = false;
bool parsingWhile = false;

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
        var_names[variables] = var_name;

        advance_symbol(tokens);

        if (current_token->type != TOK_EQUALS) {
            printf("Expected =\n");
            exit(1);
        }
        advance_symbol(tokens);
        NodeReturn var_expression = expression(tokens);

        if (var_expression.node_type == STRING) {
            var_types[variables] = "string";
        } 
        else {
            var_types[variables] = "binop";
        }

        NodeReturn variable = create_var_assign_node(var_name, var_expression);
        variables++;
        return variable;
    }
    else if (strcmp(current_token->value, "while") == 0) {
        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_PARENTHESES) {
            printf("Expected (\n");
            exit(1);
        }
        advance_symbol(tokens);

        PUSH(current_token->position);

        NodeReturn while_expression = expression(tokens);
        NodeReturn while_statement = create_while_node(while_expression, PEEK());
        if (current_token->type != TOK_CLOSE_PARENTHESES) {
            printf("Expected )\n");
            exit(1);
        }

        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_CURLY_BRACE) {
            printf("Expected {\n");
            exit(1);
        }
        parsingWhile = true;
        PUSHWHILE(1);
        return while_statement;
    }
    else if (strcmp(current_token->value, "if") == 0) {
        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_PARENTHESES) {
            printf("Expected (\n");
            exit(1);
        }

        advance_symbol(tokens);

        //if_statement_stack++;
        PUSH(current_token->position);
        NodeReturn if_expression = expression(tokens);
        NodeReturn if_statement = create_if_node(if_expression, PEEK()); //if statement stack can be thought of as a pointer to the top of the stack
        if (current_token->type != TOK_CLOSE_PARENTHESES) {
            printf("Expected )\n");
            exit(1);
        }

        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_CURLY_BRACE) {
            printf("Expected {\n");
            exit(1);
        }
        parsingIf = true;
        PUSHIF(1);
        return if_statement;
    }   
    else if (strcmp(current_token->value, "}") == 0) {
        int stack_pos = POP();
        // if (parsingIf == true) {
        //     NodeReturn res = create_end_node(stack_pos, IF);
        //     parsingIf = false;
        //     return res;
        // }
        // else if (parsingWhile == true) {
        //     NodeReturn res = create_end_node(stack_pos, WHILE);
        //     parsingWhile = false;
        //     return res;
        // }
        // // else {
        // //     printf("Sorry an error has occurred\n");
        // //     exit(1);
        // // }

        if (if_stack_pointer > 0) {
            printf("IN AN IF STATEMENT\n");
            int dropped = POPIF();

            NodeReturn res = create_end_node(stack_pos, IF);
            return res;
        }
        else if (while_stack_pointer > 0) {
            printf("IN AN WHILE STATEMENT\n");
            int dropped = POPWHILE();
            NodeReturn res = create_end_node(stack_pos, WHILE);
            return res;
        }
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

        left = create_bin_op_node(left, op, right, PEEK());
    }
    return left;
}

void visit_node(NodeReturn node);
NodeType visit_node_and_get_type(NodeReturn node);
 
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
    
    NodeType left_type = visit_node_and_get_type(left);

    Token op = bin_op->op;
    printf(" %s ", op.value);

    NodeReturn right = bin_op->right;

    NodeType right_type = visit_node_and_get_type(right);

    if (op.type == TOK_PLUS) {
        codegen_add(node);
    }
    else if (op.type == TOK_MULT) {
        codegen_mult(node);
    }    
    else if (op.type == TOK_NOT_EQUAL) {
        if (left_type == NUMBER || right_type == NUMBER) {
            codegen_not_equal(node);
        }
        else if (left_type == STRING || right_type == STRING) {
            codegen_string_not_equal(node);
        }
        else if (left_type == USEVAR) {
            UseVar *left_use_var = (UseVar *)left.node;
            if (var_types[left_use_var->index] == "string") {
                codegen_string_not_equal(node);
            }
            else {
                codegen_not_equal(node);
            }
        }
        else if (right_type == USEVAR) {
            UseVar *right_use_var = (UseVar *)right.node;
            if (var_types[right_use_var->index] == "string") {
                codegen_string_not_equal(node);
            }
            else {
                codegen_not_equal(node);
            }
        }
    }

    printf(")");
}

void visit_if_node(NodeReturn node) {
    IfNode *var = (IfNode *)node.node;
    NodeReturn expr = var->expression;
    printf("IF: ");

    visit_node(expr);
    codegen_if(node);
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

void visit_string_node(NodeReturn node) {
    String *string = (String *)node.node;
    printf(" %s ", string->value);
    codegen_string(node);
}

void visit_stdout_node(NodeReturn node) {
    StdOut *stdout = (StdOut *)node.node;
    NodeReturn expr = stdout->expression;

    printf("STDOUT: ");
    visit_node(expr);
    codegen_stdout(expr);
}

void visit_while_node(NodeReturn node) {
    WhileNode *while_node = (WhileNode *)node.node;
    NodeReturn expr = while_node->expression;
    printf("WHILE: ");

    visit_node(expr);
    codegen_while_node(node);
}

NodeType visit_node_and_get_type(NodeReturn node) {
    if (node.node_type == BINOP) {
        visit_binop(node);
        return node.node_type;
    }
    else if (node.node_type == USEVAR) {
        visit_use_var(node);
        return node.node_type;
    }
    else if (node.node_type == NUMBER) {
        visit_number(node);
        return node.node_type;
    }
    else if (node.node_type == VARASSIGN) {
        visit_var_assign_node(node);
        return node.node_type;
    }
    else if (node.node_type == STDOUT) {
        visit_stdout_node(node);
        return node.node_type;
    }  
    else if (node.node_type == IF) {
        visit_if_node(node);
        return node.node_type;
    }  
    else if (node.node_type == END) {
        visit_end_node(node);
        return node.node_type;
    }  
    else if (node.node_type == STRING) {
        visit_string_node(node);
        return node.node_type;
    }
    else if (node.node_type == WHILE) {
        visit_while_node(node);
        return node.node_type;
    }
    else {
        printf("Unknown type: %d\n", node.node_type);
    }
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
    else if (node.node_type == STRING) {
        visit_string_node(node);
    }
    else if (node.node_type == WHILE) {
        visit_while_node(node);
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
    setup_stack();
    codegen_setup();
    for (int i = 0; i < 17; i++) {
        generate_and_visit_node(tokens);
    }
    codegen_end();
}
