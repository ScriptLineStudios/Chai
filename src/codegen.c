#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "parser.h"
#include "lexer.h"

int pushes;
FILE *file_ptr;
void codegen_setup(NodeReturn node) {
    file_ptr = fopen("code.asm", "w");

    fprintf(file_ptr, "global main\n");
    fprintf(file_ptr, "extern printf\n");
    fprintf(file_ptr, "section .text\n");
    fprintf(file_ptr, "main:\n");
    fprintf(file_ptr, "    sub rsp, 32\n");
}

int tok_markers = 0;

void codegen_number(NodeReturn node) {
    Number *number = (Number *)node.node;
    fprintf(file_ptr, "    push %d\n", number->value);
    tok_markers++;
}

void codegen_add(NodeReturn node) {
    Number *number = (Number *)node.node;
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    pop rbx\n");
    fprintf(file_ptr, "    add rax, rbx\n");
    fprintf(file_ptr, "    push rax\n");
    tok_markers++;
}

void codegen_mult(NodeReturn node) {
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    pop rdx\n");
    fprintf(file_ptr, "    mul rdx\n");
    fprintf(file_ptr, "    push rax\n");
    tok_markers++;
}

void codegen_var_use(NodeReturn node) {
    UseVar *use_var = (UseVar *)node.node;
    fprintf(file_ptr, "    mov rax, [x+%d]\n", (use_var->index) * 8);
    fprintf(file_ptr, "    push rax\n");
    tok_markers++;
}

void codegen_var(NodeReturn node) {
    VarAssign *var = (VarAssign *)node.node;
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    mov [x+%d], rax\n", (var->index) * 8); //push the value onto the stack
}


#define CMP_BUFFER 65525
char *cmp_buffer;

int cleanups = 0;

void codegen_string_not_equal(NodeReturn node) {
    BinOp *bin_op = (BinOp *)node.node;
    
    NodeReturn left = bin_op->left;
    NodeReturn right = bin_op->right;

    fprintf(file_ptr, "comp_%d:\n", bin_op->stack_pos); //Get the addresses of the 2 strings
    visit_node(left);
    visit_node(right);

    fprintf(file_ptr, "    pop rax\n"); //Get the addresses of the 2 strings
    fprintf(file_ptr, "    pop rbx\n"); //Get the addresses of the 2 strings

    fprintf(file_ptr, "    mov rsi, rax\n");
    fprintf(file_ptr, "    mov rdi, rbx\n");
    fprintf(file_ptr, "    mov ecx, 6\n");

    
    fprintf(file_ptr, "    repe cmpsb\n");
    fprintf(file_ptr, "    jne branch_%d\n", bin_op->stack_pos);
    fprintf(file_ptr, "    je end_branch_%d\n", bin_op->stack_pos);
    cleanups+=2;
}

#define MAX_TEMP_BUFFER_SIZE 128

void codegen_not_equal(NodeReturn node) {
    BinOp *bin_op = (BinOp *)node.node;

    NodeReturn left = bin_op->left;
    NodeReturn right = bin_op->right;

    fprintf(file_ptr, "comp_%d:\n", bin_op->stack_pos); //Get the addresses of the 2 strings
    visit_node(left);
    visit_node(right);

    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    pop rbx\n");
    fprintf(file_ptr, "    cmp rax, rbx\n");    
    fprintf(file_ptr, "    jne branch_%d\n", bin_op->stack_pos);
    fprintf(file_ptr, "    je end_branch_%d\n", bin_op->stack_pos);
    cleanups += 2;
}

void codegen_while_node(NodeReturn node) {
    WhileNode *while_node = (WhileNode *)node.node;
    fprintf(file_ptr, "branch_%d:\n", while_node->stack_pos);
}

void codegen_if(NodeReturn node) {
    IfNode *if_node = (IfNode *)node.node;
    fprintf(file_ptr, "branch_%d:\n", if_node->stack_pos);
}


void codegen_end_node(NodeReturn node) {
    End *end_node = (End *)node.node;
    if (end_node->ending == IF) {
        fprintf(file_ptr, "end_branch_%d:\n", end_node->stack_pos);
    }
    else if (end_node->ending == WHILE) {
        fprintf(file_ptr, "    jmp comp_%d\n", end_node->stack_pos);
        fprintf(file_ptr, "end_branch_%d:\n", end_node->stack_pos);
    }
}

void codegen_stdout(NodeReturn node) {
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    mov rdi, format\n");
    fprintf(file_ptr, "    mov rsi, rax\n");
    fprintf(file_ptr, "    xor rax, rax\n");
    fprintf(file_ptr, "    call printf\n");
}

char *strings[6400000] = {0}; 
int number_alloced_strings = 0;

void queue_string_data_gen(char *value) {
    strings[number_alloced_strings] = value;
    number_alloced_strings++;
}

void codegen_string(NodeReturn node) {
	String *string = (String *)node.node;
    queue_string_data_gen(string->value);
    fprintf(file_ptr, "    push string_%d\n", number_alloced_strings - 1);
}

void codegen_end() {
    for (int i = 0; i < cleanups; i++) {
        fprintf(file_ptr, "    pop rax\n");
    }
    fprintf(file_ptr, "    add rsp, 32\n");
    fprintf(file_ptr, "    ret\n");
    fprintf(file_ptr, "format:\n");
    fprintf(file_ptr, "    db %s, 10, 0", "\"%d\"");   
    for (int i = 0; i < number_alloced_strings; i++) {
        fprintf(file_ptr, "\nstring_%d:\n", i);
        fprintf(file_ptr, "    db \"%s\", %d, 0\n", strings[i], strlen(strings[number_alloced_strings - 1]));
    }
    fprintf(file_ptr, "\nsection .data\n");
    fprintf(file_ptr, "    x db 0\n");
}
