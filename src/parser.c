#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>

#include "lexer.h"
#include "codegen.h"
#include "error.h"

int token_index = 0;
int if_statement_stack = -1;

Token *current_token;
int number_tokens = 0;

int stack_pointer;
int *branch_stack;

int if_stack_pointer;
int *if_stack;

int while_stack_pointer;
int *while_stack;

bool compile_complete = false;

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
    if (current_token->position + 1 == number_tokens) {
        compile_complete = true;
    }
}
void decrement_symbol(Token *tokens) {
    token_index--;
    current_token = tokens+token_index;
}

Token *token_peek(Token *tokens) {
    return (tokens+token_index); 
}

Token *prev_token(Token *tokens) {
    return (tokens+token_index-2);
}

NodeReturn return_node(void *node, NodeType node_type) {
    NodeReturn ret;
    ret.node = node;
    ret.node_type = node_type;
    ret.position = current_token->position;

    return ret;
}

int var_index = 0;

NodeReturn create_function_call_node(char *func_name, NodeReturn func_args, int num_args){
    FunctionCall *function_call = malloc(sizeof(FunctionCall));

    function_call->function_name = func_name;
    function_call->args = func_args;
    function_call->num_args = num_args;

    return return_node((void *)function_call, FUNCTIONCALL);
}

NodeReturn create_function_node(NodeReturn *expressions, int num_expressions) {
    Function *function = (Function *)malloc(sizeof(Function));

    function->expressions = expressions;
    function->num_expressions = num_expressions;
    return return_node((void *)function, FUNCTION);
}

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

char *func_arg_types[100];
char *function_arguments[100];
int num_func_args = 0;

char *function_names[100];
int num_functions = 0;

int isfunction(char *input) {
    for (int i = 0; i < num_functions; i++) {
        if (strcmp(input, function_names[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int isfunctionarg(char *input) {
    for (int i = 0; i < num_func_args; i++) {
        if (strcmp(input, function_arguments[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

int getfunctionarg(char *input) {
    for (int i = 0; i < num_func_args; i++) {
        if (strcmp(input, function_arguments[i]) == 0) {
            return i;
        }
    }
    return 0;
}

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

static bool is_parsing_function = false;

NodeReturn expression(Token *tokens);

NodeReturn factor(Token *tokens) {
    Number *number = (Number*)malloc(sizeof(Number));

    if (current_token->type == TOK_NUMBER) {
        number->value = atoi(current_token->value);
        advance_symbol(tokens);

        return return_node(number, NUMBER);
    }

    else if (is_parsing_function && isfunctionarg(current_token->value)) {
        UseVar *use_var = (UseVar*)malloc(sizeof(UseVar));
        use_var->name = current_token->value;
        use_var->index = getfunctionarg(current_token->value);
        advance_symbol(tokens);
        if (strcmp(current_token->value, "[") == 0) {
            advance_symbol(tokens);
            NodeReturn expr = expression(tokens);
            if (strcmp(current_token->value, "]") != 0) {
                printf("Expected ]\n");
                exit(1);
            }
            advance_symbol(tokens);
            if (strcmp(current_token->value, "=") == 0) {
                ListReassign *list_reassign = (ListReassign*)malloc(sizeof(ListReassign));
                advance_symbol(tokens);
                NodeReturn assignment = expression(tokens);
                if (strcmp(current_token->value, ";") != 0) {
                    printf("Expected semicolon!");
                    exit(1);
                }
                list_reassign->expression = expr;
                list_reassign->assignment = assignment;
                list_reassign->name = use_var->name;
                list_reassign->index = use_var->index;
                return return_node(list_reassign, LISTREASSIGN);
            }
            else {
                ListAccess *list_access = (ListAccess*)malloc(sizeof(ListAccess));
                list_access->expression = expr;
                list_access->name = use_var->name;
                list_access->index = use_var->index;
                return return_node(list_access, LISTACCESS);
            }
        }
        return return_node(use_var, USEVAR);        
    }

    else if (isvariable(current_token->value) == 1) {
        UseVar *use_var = (UseVar*)malloc(sizeof(UseVar));
        use_var->name = current_token->value;
        use_var->index = getvariableindex(current_token->value);
        advance_symbol(tokens);
        if (strcmp(current_token->value, "[") == 0) {
            advance_symbol(tokens);
            NodeReturn expr = expression(tokens);
            if (strcmp(current_token->value, "]") != 0) {
                printf("Expected ]\n");
                exit(1);
            }
            advance_symbol(tokens);
            if (strcmp(current_token->value, "=") == 0) {
                ListReassign *list_reassign = (ListReassign*)malloc(sizeof(ListReassign));
                advance_symbol(tokens);
                NodeReturn assignment = expression(tokens);
                if (strcmp(current_token->value, ";") != 0) {
                    printf("Expected semicolon!");
                    exit(1);
                }
                list_reassign->expression = expr;
                list_reassign->assignment = assignment;
                list_reassign->name = use_var->name;
                list_reassign->index = use_var->index;
                return return_node(list_reassign, LISTREASSIGN);
            }
            else {
                ListAccess *list_access = (ListAccess*)malloc(sizeof(ListAccess));
                list_access->expression = expr;
                list_access->name = use_var->name;
                list_access->index = use_var->index;
                return return_node(list_access, LISTACCESS);
            }
        }

        return return_node(use_var, USEVAR);
    }

    else if (current_token->type == TOK_STRING) {
        String *string = malloc(sizeof(String));
        string->value = current_token->value;
        advance_symbol(tokens);

        return return_node(string, STRING);
    }

    printf("\nCould not find type: %s\n", current_token->value);
    raise_error(prev_token(tokens), "An error was raised above!");
//    return return_node(NULL, NULL_TYPE);
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

int is_int(char *scan) {
    int len = strlen(scan);
    for (int i = 0; i < len; i++) {
        if (!isdigit(scan[i])) {
            return 0;
        }
    }

    return 1;
}

List *lists[1000];
int num_lists = 0;

NodeReturn update_old_list(Token *tokens, List *list) {
    list->size = 0;
    list->values = realloc(list->values, list->size * sizeof(char **));

    advance_symbol(tokens);
    while (strcmp(current_token->value, ";") != 0) {
        if (is_int(current_token->value)) {
            list->values[list->size] = current_token->value;
            list->size++;
            list->values = realloc(list->values, list->size * sizeof(char **));
        }
        advance_symbol(tokens);
    }
}

NodeReturn create_new_list(Token *tokens) {
    printf("----- Beginning Search For New List Elements! -----\n");
    
    List *list = (List*)malloc(sizeof(List));
    list->size = 0;
    list->values = malloc(sizeof(char **) * 100);
    
    advance_symbol(tokens);
    while (strcmp(current_token->value, ";") != 0) {
        if (is_int(current_token->value)) {
            assert(list->size < 100);
            list->values[list->size] = current_token->value;
            list->size++;
        }
        advance_symbol(tokens);
    }
    lists[variables] = list;
    return return_node((void *)list, LIST);
}

char **get_function_args(Token *tokens) {
    printf("beginning search for function arguments current token = %s\n", current_token->value);

    char **args = malloc(sizeof(char **) * 100);
    int arg_size = 0;
    advance_symbol(tokens);
    while (strcmp(current_token->value, ")") != 0) {
        if (strcmp(current_token->value, ",") != 0) {
            assert(arg_size < 100);
            function_arguments[num_func_args] = current_token->value;
            args[arg_size] = current_token->value;
            func_arg_types[num_func_args] = current_token->value; //TODO: THESE NEED REAL TYPES!!!!
            arg_size++;
            num_func_args++;
        }
        advance_symbol(tokens);
    }
    printf("---- ending search sucessfully found %d arguments ----\n", arg_size);
    return args;
}

NodeReturn expression(Token *tokens) {
    if (strcmp(current_token->value, "let") == 0) {
        advance_symbol(tokens);
        if (current_token->type != TOK_IDENT) {
            raise_error(prev_token(tokens), "Expected Identifier");
        }

        char *var_name = current_token->value;
        var_names[variables] = var_name;

        advance_symbol(tokens);

        if (current_token->type != TOK_EQUALS) {
            raise_error(prev_token(tokens), "Expected =");
            exit(1);
        }
        advance_symbol(tokens);
        
        NodeReturn var_expression;
        if (current_token->type == TOK_OPEN_SQUARE_BRACKET) {
            printf("Found the start of a list!\n");
            var_expression = create_new_list(tokens);
        }
        else {
            var_expression = expression(tokens);
        }
    
        if (var_expression.node_type == STRING) {
            var_types[variables] = "string";
        } 
        else if (var_expression.node_type == LIST) {
            var_types[variables] = "list";
        }
        else {
            var_types[variables] = "binop";
        }

        if (current_token->type != TOK_SEMI) {
            raise_error(prev_token(tokens), "Expected ;");
        }

        NodeReturn variable = create_var_assign_node(var_name, var_expression);
        variables++;
        return variable;
    }

    else if (strcmp(current_token->value, "func") == 0) {
        advance_symbol(tokens);
        if (current_token->type != TOK_IDENT) {
            raise_error(prev_token(tokens), "Expected identifier");
        }
        printf("FUNCTION NAME = %s\n", current_token->value);

        char *function_name = current_token->value;
        function_names[num_functions] = function_name;
        num_functions++;

        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_PARENTHESES) {
            raise_error(prev_token(tokens), "Expected (");
        }

        char **function_args = get_function_args(tokens);
        advance_symbol(tokens);

        if (current_token->type != TOK_OPEN_CURLY_BRACE) {
            raise_error(prev_token(tokens), "Expected {");
        }

        is_parsing_function = true;
        NodeReturn *function_experessions = malloc(sizeof(NodeReturn) * 100);
        int number_experessions = 0;
        
        while (current_token->type != TOK_CLOSE_CURLY_BRACE) {  
            advance_symbol(tokens);
            printf("going into search with: %s\n", current_token->value);
            if (current_token->type == TOK_CLOSE_CURLY_BRACE && if_stack_pointer <= 0 && while_stack_pointer <= 0) {
                break;
            }  //Making sure we are closing of the right bracket
            function_experessions[number_experessions++] = expression(tokens);
        }

        printf("got an expression! current_token: %s\n", current_token->value);
        NodeReturn function = create_function_node(function_experessions, number_experessions);
        return function;    
    }

    else if (strcmp(current_token->value, "while") == 0) {
        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_PARENTHESES) {
            raise_error(prev_token(tokens), "Expected (");
        }
        advance_symbol(tokens);

        PUSH(current_token->position);

        NodeReturn while_expression = expression(tokens);
        NodeReturn while_statement = create_while_node(while_expression, PEEK());
        if (current_token->type != TOK_CLOSE_PARENTHESES) {
            raise_error(prev_token(tokens), "Expected )");
        }

        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_CURLY_BRACE) {
            raise_error(prev_token(tokens), "Expected {");
        }
        parsingWhile = true;
        PUSHWHILE(1);
        return while_statement;
    }
    else if (isfunction(current_token->value)) {
        char *func_name = current_token->value;
        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_PARENTHESES) {
            raise_error(prev_token(tokens), "Expected (");
        }

        advance_symbol(tokens);
        NodeReturn arg; //TODO (URGENT) this needs to be an array of some sort!
        int num_func_args = 0;
        while (current_token->type != TOK_CLOSE_PARENTHESES) {
            arg = expression(tokens);
            if (current_token->type == TOK_COMMA) {
                advance_symbol(tokens);
            }
        }

        NodeReturn function_call = create_function_call_node(func_name, arg, num_func_args);

        advance_symbol(tokens);
        if (current_token->type != TOK_SEMI) {
            raise_error(prev_token(tokens), "Expected ;");
        }
        
        return function_call;
    }
    else if (strcmp(current_token->value, "if") == 0) {
        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_PARENTHESES) {
            raise_error(prev_token(tokens), "Expected (");
        }

        advance_symbol(tokens);

        //if_statement_stack++;
        PUSH(current_token->position);
        NodeReturn if_expression = expression(tokens);
        NodeReturn if_statement = create_if_node(if_expression, PEEK()); //if statement stack can be thought of as a pointer to the top of the stack
        if (current_token->type != TOK_CLOSE_PARENTHESES) {
            raise_error(prev_token(tokens), "Expected )");
        }

        advance_symbol(tokens);
        if (current_token->type != TOK_OPEN_CURLY_BRACE) {
            raise_error(prev_token(tokens), "Expected {");
        }
        parsingIf = true;
        PUSHIF(1);
        return if_statement;
    }   

    else if (strcmp(current_token->value, "}") == 0) {
        int stack_pos = POP();

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
            raise_error(prev_token(tokens), "Expected (");
        }

        advance_symbol(tokens);

        NodeReturn expr = expression(tokens);
        NodeReturn res = create_stdout_node(expr);

        if (current_token->type != TOK_CLOSE_PARENTHESES) {
            raise_error(prev_token(tokens), "Expected )");
        }

        advance_symbol(tokens);

        if (current_token->type != TOK_SEMI) {
            raise_error(prev_token(tokens), "Expected ;");
        }

        return res;
    }

    else if (isvariable(current_token->value) == 1 && token_peek(tokens)->type == TOK_EQUALS) { //this is different to checking the variable in the term since we need an entire expression
        
        char *name = current_token->value;
        advance_symbol(tokens);
        //printf("YAY: %s\n", current_token->value);
        if (current_token->type != TOK_EQUALS) {
            raise_error(prev_token(tokens), "Expected =");
        }

        advance_symbol(tokens);
        NodeReturn var_expression;

        if (current_token->type == TOK_OPEN_SQUARE_BRACKET) {
            printf("okay world");
            if (strcmp(var_types[getvariable(current_token->value)], "list") == 0) {
                var_expression = update_old_list(tokens, lists[getvariable(current_token->value)]);
            }
        }
        else {
            var_expression = expression(tokens);
        }
        NodeReturn variable = create_var_reassign_node(name, var_expression, getvariable(name));
        
        if (current_token->type != TOK_SEMI) {
            raise_error(prev_token(tokens), "Expected ;");
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

void visit_node(NodeReturn node, bool is_in_func);
NodeType visit_node_and_get_type(NodeReturn node, bool is_in_func);
 
void visit_number(NodeReturn node, bool is_in_func) {
    Number *number = (Number *)node.node;
    codegen_number(node, is_in_func);
    printf("%d", number->value);
}

void visit_use_var(NodeReturn node, bool is_in_func) {
    UseVar *use_var = (UseVar *)node.node;
    codegen_var_use(node, is_in_func);
    printf("%s", use_var->name);
}

void visit_binop(NodeReturn node, bool is_in_func) {
    printf("(");
    BinOp *bin_op = (BinOp *)node.node;    
    NodeReturn left = bin_op->left;
    
    NodeType left_type = visit_node_and_get_type(left, is_in_func);

    Token op = bin_op->op;
    printf(" %s ", op.value);

    NodeReturn right = bin_op->right;

    NodeType right_type = visit_node_and_get_type(right, is_in_func);

    if (op.type == TOK_PLUS) {
        codegen_add(node, is_in_func);
    }
    else if (op.type == TOK_MULT) {
        codegen_mult(node, is_in_func);
    }    
    else if (op.type == TOK_MINUS) {
        codegen_sub(node, is_in_func);
    }
    else if (op.type == TOK_NOT_EQUAL) {
        if (left_type == NUMBER || right_type == NUMBER) {
            codegen_not_equal(node, is_in_func);
        }
        else if (left_type == STRING || right_type == STRING) {
            codegen_string_not_equal(node, is_in_func);
        }
        else if (left_type == USEVAR) {
            UseVar *left_use_var = (UseVar *)left.node;
            if (var_types[left_use_var->index] == "string") {
                codegen_string_not_equal(node, is_in_func);
            }
            else {
                codegen_not_equal(node, is_in_func);
            }
        }
        else if (right_type == USEVAR) {
            UseVar *right_use_var = (UseVar *)right.node;
            if (var_types[right_use_var->index] == "string") {
                codegen_string_not_equal(node, is_in_func);
            }
            else {
                codegen_not_equal(node, is_in_func);
            }
        }
    }

    printf(")");
}

void visit_if_node(NodeReturn node, bool is_in_func) {
    IfNode *var = (IfNode *)node.node;
    NodeReturn expr = var->expression;
    printf("IF: ");

    visit_node(expr, is_in_func);
    codegen_if(node, is_in_func);
}

void visit_var_assign_node(NodeReturn node, bool is_in_func) {
    VarAssign *var = (VarAssign *)node.node;
    NodeReturn expr = var->expression;
    printf("VAR ASSIGN (%s): ", var->var_name);

    visit_node(expr, is_in_func);
    codegen_var(node, is_in_func);
}

void visit_end_node(NodeReturn node, bool is_in_func) {
    End *end = (End *)node.node;
    codegen_end_node(node, is_in_func);
    printf("END");
}

void visit_string_node(NodeReturn node, bool is_in_func) {
    String *string = (String *)node.node;
    printf(" %s ", string->value);
    codegen_string(node, is_in_func);
}

void visit_stdout_node(NodeReturn node, bool is_in_func) {
    StdOut *stdout = (StdOut *)node.node;
    NodeReturn expr = stdout->expression;
    printf("STDOUT: ");
    NodeType type = visit_node_and_get_type(expr, is_in_func);
    codegen_stdout(expr, type, var_types, is_in_func);
}

void visit_while_node(NodeReturn node, bool is_in_func) {
    WhileNode *while_node = (WhileNode *)node.node;
    NodeReturn expr = while_node->expression;
    printf("WHILE: ");

    visit_node(expr, is_in_func);
    codegen_while_node(node, is_in_func);
}

void visit_list_node(NodeReturn node, bool is_in_func) {
    List *list = (List *)node.node;
    printf("[");
    for (int i = 0; i < list->size; i++) {
        printf("%s, ", list->values[i]);
    }
    printf("]");
    codegen_list(node, is_in_func);
}

void visit_list_access_node(NodeReturn node, bool is_in_func) {
    ListAccess *list_access = (ListAccess *)node.node;
    NodeReturn expr = list_access->expression;

    printf(" %s->", list_access->name);
    visit_node(expr, is_in_func);
    codegen_list_access(node, is_in_func);
}

void visit_list_reassign_node(NodeReturn node, bool is_in_func) {
    ListReassign *list_reassign = (ListReassign*)node.node;
    NodeReturn expr = list_reassign->expression;
    NodeReturn assignment = list_reassign->assignment;

    printf("LIST REASSIGN (%s->", list_reassign->name);
    visit_node(expr, is_in_func);
    printf("=");
    visit_node(assignment, is_in_func);
    printf(")");
    codegen_list_reassign(node, is_in_func);
}

void visit_function_node(NodeReturn node, bool is_in_func) {
    Function *function = (Function*)node.node;
    NodeReturn *expressions = function->expressions;
    printf("FUNCTION: \n");
    codegen_function(node, is_in_func);
    for (int i = 0; i < function->num_expressions; i++) {   
        printf("    ");
        visit_node(expressions[i], true);
        printf("\n");
    }
    printf("END");
}

void visit_function_call_node(NodeReturn node, bool is_in_func) {
    FunctionCall *function_call = (FunctionCall*)node.node;
    printf("FUNCTION CALL (%s) ", function_call->function_name, function_call->num_args);
    visit_node(function_call->args, is_in_func);
    codegen_function_call(node, is_in_func);
}

NodeType visit_node_and_get_type(NodeReturn node, bool is_in_func) {
    if (node.node_type == BINOP) {
        visit_binop(node, is_in_func);
        return node.node_type;
    }
    else if (node.node_type == USEVAR) {
        visit_use_var(node, is_in_func);
        return node.node_type;
    }
    else if (node.node_type == NUMBER) {
        visit_number(node, is_in_func);
        return node.node_type;
    }
    else if (node.node_type == VARASSIGN) {
        visit_var_assign_node(node, is_in_func);
        return node.node_type;
    }
    else if (node.node_type == STDOUT) {
        visit_stdout_node(node, is_in_func);
        return node.node_type;
    }  
    else if (node.node_type == IF) {
        visit_if_node(node, is_in_func);
        return node.node_type;
    }  
    else if (node.node_type == END) {
        visit_end_node(node, is_in_func);
        return node.node_type;
    }  
    else if (node.node_type == STRING) {
        visit_string_node(node, is_in_func);
        return node.node_type;
    }
    else if (node.node_type == WHILE) {
        visit_while_node(node, is_in_func);
        return node.node_type;
    }
    else if (node.node_type == LIST) {
        visit_list_node(node, is_in_func);
        return node.node_type;
    }
    else if (node.node_type == LISTACCESS) {
        visit_list_access_node(node, is_in_func);
        return node.node_type;
    }
    else if (node.node_type == LISTREASSIGN) {
        visit_list_reassign_node(node, is_in_func);
        return node.node_type;
    }
    else if (node.node_type == FUNCTION) {
        visit_function_node(node, is_in_func);
        return node.node_type;
    }
    else if (node.node_type == FUNCTIONCALL) {
        visit_function_call_node(node, is_in_func);
        return node.node_type;
    }
    else {
        printf("Unknown type in getting type: %d\n", node.node_type);
    }
}

void visit_node(NodeReturn node, bool is_in_func) {
    if (node.node_type == BINOP) {
        visit_binop(node, is_in_func);
    }
    else if (node.node_type == USEVAR) {
        visit_use_var(node, is_in_func);
    }
    else if (node.node_type == NUMBER) {
        visit_number(node, is_in_func);
    }
    else if (node.node_type == VARASSIGN) {
        visit_var_assign_node(node, is_in_func);
    }
    else if (node.node_type == STDOUT) {
        visit_stdout_node(node, is_in_func);
    }  
    else if (node.node_type == IF) {
        visit_if_node(node, is_in_func);
    }  
    else if (node.node_type == END) {
        visit_end_node(node, is_in_func);
    }  
    else if (node.node_type == STRING) {
        visit_string_node(node, is_in_func);
    }
    else if (node.node_type == WHILE) {
        visit_while_node(node, is_in_func);
    }
    else if (node.node_type == LIST) {
        visit_list_node(node, is_in_func);
    }
    else if (node.node_type == LISTACCESS) {
        visit_list_access_node(node, is_in_func);
    }
    else if (node.node_type == LISTREASSIGN) {
        visit_list_reassign_node(node, is_in_func);
    }
    else if (node.node_type == FUNCTION) {
        visit_function_node(node, is_in_func);
    }
    else if (node.node_type == FUNCTIONCALL) {
        visit_function_call_node(node, is_in_func);
    }
    else {
        printf("Unknown type: %d\n", node.node_type);
    }
}

void generate_and_visit_node(Token *tokens) { 
    advance_symbol(tokens);
    NodeReturn node = expression(tokens);
    visit_node(node, false);

    printf("\n");
}

void generate_ast(Token *tokens, int ntokens) {
    number_tokens = ntokens;
    setup_stack();
    codegen_setup();
    printf("ntok = %d\n", ntokens);
    while (!compile_complete) 
        generate_and_visit_node(tokens);
    codegen_end();
    printf("----- COMPILATION SUCCESSFULL! -----\n");
}
