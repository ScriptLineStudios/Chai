#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#include "../include/parser.h"
#include "../include/lexer.h"
#include "../include/error.h" 
#include "../include/hashmap.h"
#include "../include/codegen.h"


static int num_declared_vars;
char **variables;

void visit_node(Node node);

void dump_vars() {
    for (int i = 0; i < num_declared_vars; i++) {
        printf("%s\n", variables[i]);
    }
}

bool is_var(char *var_name) {
    for (int i = 0; i < num_declared_vars; i++) {
        if (strcmp(var_name, variables[i]) == 0) {
            return true;
        }
    }
    return false;
}

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

Node create_var_decal_node(char *var_name, Node expression) {
    VarDecal *var_decal = malloc(sizeof(VarDecal));
    var_decal->var_name = var_name;
    var_decal->expression = expression;

    return return_node((void *)var_decal, VARDECAL);
}

Node create_var_reassign_node(char *var_name, Node expression) {
    VarReassign *var_reassign = malloc(sizeof(VarReassign));
    var_reassign->var_name = var_name;
    var_reassign->new_expression = expression;

    return return_node((void *)var_reassign, VARREASSIGN);
}

Node factor(Lexer *lexer) {
    switch (current_token.type) {
        case TOK_NUMBER:
            Number *number = malloc(sizeof(Number));
            number->value = atoi(current_token.value);
            ADVANCE;
            return return_node((void *)number, NUMBER);
        case TOK_IDENTIFIER:
            if (is_var(current_token.value)) {
                VarAccess *var_access = malloc(sizeof(VarAccess));
                var_access->var_name = current_token.value;
                ADVANCE;
                return return_node((void *)var_access, VARACCESS);
            }
            else {
                raise_compile_error(&current_token, "Unknown identifier!");
            }
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
        TOK_MATCH(TOK_IDENTIFIER, "Expected Identifier!");
        char *var_name = current_token.value;
        TOK_MATCH(TOK_EQUAL, "Expected =");

        Node var_expression = expression(lexer);
        SEMI_MATCH;
        return create_var_decal_node(var_name, var_expression);
    }       
    if (is_var(current_token.value)) {
        char *var_name = current_token.value;
        TOK_MATCH(TOK_EQUAL, "Expected =");
        Node new_expression = expression(lexer);
        SEMI_MATCH;
        return create_var_reassign_node(var_name, new_expression);
    }
}

void visit_var_reassign_node(Node node) {
    CAST(VarReassign, var_reassign);
    printf("VARIABLE REASSIGNMENT: (%s)", var_reassign->var_name);
    printf("   ");
    visit_node(var_reassign->new_expression);
}

void visit_var_access(Node node) {
    CAST(VarAccess, var_access);
    printf(" %s ", var_access->var_name);
}

void visit_var_decal(Node node) {
    CAST(VarDecal, var_decal);
    
    ALLOC_VAR(var_decal->var_name);
    printf("VARIABLE DECLARATION: (%s)", var_decal->var_name);
    printf("   ");
    visit_node(var_decal->expression);
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
    codegen_binop(node);
    printf(")"); 
}

void visit_number(Node node) {
    CAST(Number, number);
    codegen_number(node);
    printf("%d", number->value);
}

void visit_node(Node node) {
    switch (node.node_type) {
        case VARREASSIGN:
            visit_var_reassign_node(node);
            break;
        case VARACCESS:
            visit_var_access(node);
            break;
        case VARDECAL:
            visit_var_decal(node);
            break;
        case BINOP:
            visit_binop(node);
            break;
        case NUMBER: 
            visit_number(node);
            break;
    }
}

void create_node(Lexer *lexer) {
    visit_node(get_node(lexer));
    printf("\n");
}

void parse_file(Lexer *lexer) {
    codegen_init();
    variables = malloc(sizeof(char *) * num_declared_vars);
    create_node(lexer);
    codegen_end();
}