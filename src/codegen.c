//forget parser.c this file has gotten real messy. TODO: clean this mess up.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>

#include "parser.h"
#include "lexer.h"

int pushes;
FILE *file_ptr;

FILE *get_file_ptr() {
    return file_ptr;
}

char *function_lines[6400000];
int function_line_index = 0;

int queue_function_line_gen(FILE *temp, const char *line, ...) {
    char *str = malloc(sizeof(char*) * 100);

    va_list ptr;
    va_start(ptr, line);
    
    //thanks https://stackoverflow.com/questions/43129133/c-variable-argument-with-sprintf for stopping me from using sprintf :)
    vsnprintf(str, 100, line, ptr); 

    function_lines[function_line_index++] = str;
    va_end(ptr);
    return 0;
}

int (*queue_ptr)(FILE *, const char *, ...);
int (*print_ptr)(FILE *, const char *, ...);
int (*print_choice[2])(FILE *, const char *, ...);
void codegen_setup(NodeReturn node) {
    queue_ptr = &queue_function_line_gen;
    print_ptr = &fprintf;

    print_choice[0] = queue_ptr;
    print_choice[1] = print_ptr;

    file_ptr = fopen("code.asm", "w");

    fprintf(file_ptr, "global main\n");
    fprintf(file_ptr, "extern printf\n");
    fprintf(file_ptr, "section .text\n");
    fprintf(file_ptr, "main:\n");
}

int tok_markers = 0;

#define WRITE print_choice[(int)!is_in_func]

static bool parsing_args = false;
static int arg_num; 

void codegen_set_parsing_args(bool value) {
    parsing_args = value;
}

void codegen_set_arg_num(int value) {
    arg_num = value;
}

// thanks to http://6.s081.scripts.mit.edu/sp18/x86-64-architecture-guide.html for providing this order.
char *argument_registers[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"}; //for now we do not support stack based arguments :(

void codegen_number_arg(NodeReturn node, bool is_in_func, int arg_num) {
    WRITE(file_ptr, ";; --- Number Arg\n");
    Number *number = (Number*)node.node;
    WRITE(file_ptr, "    mov %s, %d\n", argument_registers[arg_num], number->value);
}

void codegen_number(NodeReturn node, bool is_in_func) {
    if (parsing_args) {
        codegen_number_arg(node, is_in_func, arg_num);
        arg_num++;
    }
    else {
        WRITE(file_ptr, ";; --- Number\n");
        Number *number = (Number *)node.node;
        WRITE(file_ptr, "    push %d\n", number->value);
        tok_markers++;
    }
}


void codegen_add(NodeReturn node, bool is_in_func) {
    Number *number = (Number *)node.node;
    WRITE(file_ptr, "; ---- ADD \n");
    WRITE(file_ptr, "    pop rax\n");
    WRITE(file_ptr, "    pop rbx\n");
    WRITE(file_ptr, "    add rax, rbx\n");
    WRITE(file_ptr, "    push rax\n");
    tok_markers++;
}

void codegen_sub(NodeReturn node, bool is_in_func) {
    Number *number = (Number *)node.node;
    WRITE(file_ptr, "; ---- SUB \n");
    if (parsing_args) {
        WRITE(file_ptr, "    mov rax, %s\n", argument_registers[arg_num-1]);
        WRITE(file_ptr, "    mov rbx, %s\n", argument_registers[arg_num-2]);
        WRITE(file_ptr, "    sub rbx, rax\n");
        WRITE(file_ptr, "    mov %s, rbx\n", argument_registers[arg_num-2]);
        arg_num -= 2;
    }
    else {
        WRITE(file_ptr, "    pop rax\n");
        WRITE(file_ptr, "    pop rbx\n");
        WRITE(file_ptr, "    sub rbx, rax\n");
        WRITE(file_ptr, "    push rbx\n");
    }
    tok_markers++;
}

void codegen_mult(NodeReturn node, bool is_in_func) {
    WRITE(file_ptr, "    pop rax\n");
    WRITE(file_ptr, "    pop rdx\n");
    WRITE(file_ptr, "    mul rdx\n");
    WRITE(file_ptr, "    push rax\n");
    tok_markers++;
}

// void codegen_number_arg(NodeReturn node, bool is_in_func, int arg_num) {
//     WRITE(file_ptr, ";; --- Number Arg\n");
//     Number *number = (Number*)node.node;
//     WRITE(file_ptr, "    mov %s, %d\n", argument_registers[arg_num], number->value);
// }

void codegen_use_var_arg(NodeReturn node, bool is_in_func, int arg_num, int index) {
    WRITE(file_ptr, "    mov rax");
}

int num_list_offset = -1;
void codegen_var_use(NodeReturn node, bool is_in_func) {
    WRITE(file_ptr, "; ---- Use Var\n");
    UseVar *use_var = (UseVar *)node.node;
    // if (is_in_func && isfunctionarg(use_var->name)) {
    //     //TODO these must be registers.
    //     WRITE(file_ptr, "    mov rax, %s\n", argument_registers[getfunctionarg(use_var->name)]);
    // }
    // else {
    //} Thank goodness this is gone
    WRITE(file_ptr, "    mov rax, [x+%d]\n", (use_var->index+num_list_offset) * 8);
    if (parsing_args) {
        WRITE(file_ptr, "    mov %s, [x+%d]\n", argument_registers[arg_num], (use_var->index+num_list_offset) * 8);
        arg_num++;
    }
    else {
        WRITE(file_ptr, "    push rax\n");
    }
    tok_markers++;
}

void codegen_var_argument(NodeReturn node, bool is_in_func, int index) {
    //TODO: This function needs fixing 2022/11/18
    WRITE(file_ptr, ";; --- Var\n");

    VarAssign *var = (VarAssign *)node.node;
    NodeReturn expr = var->expression;
    if (expr.node_type == LIST) {
        List *list = (List *)expr.node;
        for (int i = 0; i < list->size; i++) {
            WRITE(file_ptr, "    pop rax\n");
            WRITE(file_ptr, "    mov [x+%d], rax\n", (var->index+i) * 8); //push the value onto the stack
            num_list_offset++;
        }
    }
    else {
        WRITE(file_ptr, "    mov [x+%d], %s\n", (var->index+num_list_offset) * 8, argument_registers[index]); //push the value onto the stack
    }
}

void codegen_var(NodeReturn node, bool is_in_func) {
    WRITE(file_ptr, ";; --- Var\n");

    VarAssign *var = (VarAssign *)node.node;
    NodeReturn expr = var->expression;
    if (expr.node_type == LIST) {
        List *list = (List *)expr.node;
        for (int i = 0; i < list->size; i++) {
            WRITE(file_ptr, "    pop rax\n");
            WRITE(file_ptr, "    mov [x+%d], rax\n", (var->index+i) * 8); //push the value onto the stack
            num_list_offset++;
        }
    }
    else {
        WRITE(file_ptr, "    pop rax\n");
        WRITE(file_ptr, "    mov [x+%d], rax\n", (var->index+num_list_offset) * 8); //push the value onto the stack
    }
}

#define CMP_BUFFER 65525
char *cmp_buffer;

int cleanups = 0;

void codegen_string_not_equal(NodeReturn node, bool is_in_func) {
    BinOp *bin_op = (BinOp *)node.node;
    
    NodeReturn left = bin_op->left;
    NodeReturn right = bin_op->right;

    WRITE(file_ptr, "comp_%d:\n", bin_op->stack_pos); //Get the addresses of the 2 strings
    visit_node(left, is_in_func);
    visit_node(right, is_in_func);

    WRITE(file_ptr, "    pop rax\n"); //Get the addresses of the 2 strings
    WRITE(file_ptr, "    pop rbx\n"); //Get the addresses of the 2 strings

    WRITE(file_ptr, "    mov rsi, rax\n");
    WRITE(file_ptr, "    mov rdi, rbx\n");
    WRITE(file_ptr, "    mov ecx, 6\n");

    WRITE(file_ptr, "    repe cmpsb\n");
    WRITE(file_ptr, "    jne branch_%d\n", bin_op->stack_pos);
    WRITE(file_ptr, "    je end_branch_%d\n", bin_op->stack_pos);
    cleanups+=2;
}

#define MAX_TEMP_BUFFER_SIZE 128

void codegen_not_equal(NodeReturn node, bool is_in_func) {
    BinOp *bin_op = (BinOp *)node.node;

    NodeReturn left = bin_op->left;
    NodeReturn right = bin_op->right;

    WRITE(file_ptr, "comp_%d:\n", bin_op->stack_pos); //Get the addresses of the 2 strings
    visit_node(left, is_in_func);
    visit_node(right, is_in_func);

    WRITE(file_ptr, "    pop rax\n");
    WRITE(file_ptr, "    pop rbx\n");
    WRITE(file_ptr, "    cmp rax, rbx\n");    
    WRITE(file_ptr, "    jne branch_%d\n", bin_op->stack_pos);
    WRITE(file_ptr, "    je end_branch_%d\n", bin_op->stack_pos);
    cleanups += 2;
}

void codegen_while_node(NodeReturn node, bool is_in_func) {
    WhileNode *while_node = (WhileNode *)node.node;
    WRITE(file_ptr, "branch_%d:\n", while_node->stack_pos);
}

void codegen_if(NodeReturn node, bool is_in_func) {
    IfNode *if_node = (IfNode *)node.node;
    WRITE(file_ptr, "branch_%d:\n", if_node->stack_pos);
}

void codegen_end_node(NodeReturn node, bool is_in_func) {
    End *end_node = (End *)node.node;
    if (end_node->ending == IF) {
        WRITE(file_ptr, "end_branch_%d:\n", end_node->stack_pos);
    }
    else if (end_node->ending == WHILE) {
        WRITE(file_ptr, "    jmp comp_%d\n", end_node->stack_pos);
        WRITE(file_ptr, "end_branch_%d:\n", end_node->stack_pos);
    }
}

#define CODEGEN_SAVE_REGISTER_ARGS  for (int i = 0; i < sizeof(argument_registers) / sizeof(argument_registers[0]); i++) {   \
    WRITE(file_ptr, "push %s\n", argument_registers[i]);                                                                     \
} \

#define CODEGEN_RESTIRE_REGISTER_ARGS    for (int i = sizeof(argument_registers) / sizeof(argument_registers[0]); i > 0; i++) {  \
        WRITE(file_ptr, "pop %s\n", argument_registers[i]);                                                                      \
    }                                                                                                                             

void codegen_stdout(NodeReturn node, NodeType type, char **var_types, bool is_in_func) {
    WRITE(file_ptr, ";; --- Stdout Start\n");


    WRITE(file_ptr, "    pop rax\n");

    if (type == NUMBER) {
        printf("here");
        WRITE(file_ptr, "    mov rdi, format\n");
    }
    else if (type == USEVAR) {
        UseVar *use_var = (UseVar *)node.node;
        if (var_types[getvariableindex(use_var->name)] == "string") { //TODO: REIMPLMENT THIS: || func_args_types[getfunctionarg(use_var->name)] == "string") {
            WRITE(file_ptr, "    mov rdi, string_format\n");
        }
        else {
            WRITE(file_ptr, "    mov rdi, format\n");
        }
    }
    else if (type == LISTACCESS) {
        WRITE(file_ptr, "    mov rdi, format\n");
    }
    else {
        WRITE(file_ptr, "    mov rdi, string_format\n");
    }

    WRITE(file_ptr, "    mov rsi, rax\n");
    WRITE(file_ptr, "    mov al,0\n");
    WRITE(file_ptr, "    xor rax, rax\n");
    WRITE(file_ptr, "    call printf\n");
}

char *strings[6400000] = {0}; 
int number_alloced_strings = 0;

void queue_string_data_gen(char *value) {
    strings[number_alloced_strings] = value;
    number_alloced_strings++;
}

void codegen_string(NodeReturn node, bool is_in_func) {
	String *string = (String *)node.node;
    queue_string_data_gen(string->value);
    WRITE(file_ptr, "    push string_%d\n", number_alloced_strings - 1);
}

void codegen_list(NodeReturn node, bool is_in_func) {
    List *list = (List *)node.node;

    for (int i = list->size - 1; i >= 0; i--) {
        WRITE(file_ptr, "    push %d\n", atoi(list->values[i]));
    }
}

void codegen_list_access(NodeReturn node, bool is_in_func) {
    WRITE(file_ptr, ";; --- List Access\n");
    ListAccess *list_access = (ListAccess *)node.node;
    WRITE(file_ptr, "    pop rbx\n");
    WRITE(file_ptr, "    mov rax, [x+%d+rbx*8]\n", (list_access->index) * 8);
    WRITE(file_ptr, "    push rax\n");
}

void codegen_list_reassign(NodeReturn node, bool is_in_func) {
    ListReassign *list_reassign = (ListReassign *)node.node;
    WRITE(file_ptr, ";; --- List Reassign\n");
    WRITE(file_ptr, "    pop rax\n"); // this is the value
    WRITE(file_ptr, "    pop rbx\n"); // this is the index
    WRITE(file_ptr, "    mov [x+%d+rbx*8], rax\n", (list_reassign->index) * 8);
}

char *functions[100];
int allocated_functions = 0;

void queue_function_name_gen() {
    //TODO: this should accept a name
    functions[allocated_functions++] = "function:\n";
}

void codegen_function(NodeReturn node, bool is_in_func) {
    Function *function = (Function *)node.node;
    queue_function_name_gen(function); //TODO: this should pass name
}

void codegen_function_call(NodeReturn node, bool is_in_func) {
    FunctionCall *function_call = (FunctionCall*)node.node;
    WRITE(file_ptr, "    call %s\n", function_call->function_name);
}

void codegen_extern_node(NodeReturn node, bool is_in_func) {
    ExternNode *extern_node = (ExternNode*)node.node;
    WRITE(file_ptr, "    extern %s\n", extern_node->name);
}

void codegen_return(NodeReturn node, bool is_in_func) {
    ReturnNode *return_node = (ReturnNode *)node.node;

    if (return_node->return_expr.node_type != NULL_TYPE) {
        WRITE(file_ptr, "    pop rax\n");
    }
    WRITE(file_ptr, "    mov [rsp+8], rax\n");

    for (int i = 0; i < cleanups; i++) {
        WRITE(file_ptr, "    pop rbx\n");
    }

    WRITE(file_ptr, "    ret\n");
}

void codegen_end() {
    fprintf(file_ptr, "    mov rax, 60\n");
    fprintf(file_ptr, "    syscall\n");

    fprintf(file_ptr, "    ret\n");

    for (int i = 0; i < allocated_functions; i++) {
        fprintf(file_ptr, functions[i]);
    }

    for (int i = 0; i < function_line_index; i++) {
        fprintf(file_ptr, function_lines[i]);
        //free(function_lines[i]);
    }
    
    fprintf(file_ptr, "format:\n");
    fprintf(file_ptr, "    db %s, 10, 0\n", "\"%d\"");
    fprintf(file_ptr, "string_format:\n");
    fprintf(file_ptr, "    db %s, 10, 0\n", "\"%s\"");   
    
    for (int i = 0; i < number_alloced_strings; i++) {
        fprintf(file_ptr, "\nstring_%d:\n", i);
        //fprintf(file_ptr, "    db `%s`, %d, 0\n", strings[i], strlen(strings[number_alloced_strings - 1]));
        fprintf(file_ptr, "    db `%s`, %d, 0\n", strings[i], 9);
    
    }
    fprintf(file_ptr, "\nsection .data\n");
    fprintf(file_ptr, "    x db 0\n");
    fclose(file_ptr);
}
