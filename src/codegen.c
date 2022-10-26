#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

int pushes;
FILE *file_ptr;
void codegen_setup(NodeReturn node) {
    file_ptr = fopen("code.asm", "w");

    fprintf(file_ptr, "global main\n");
    fprintf(file_ptr, "extern printf\n");
    fprintf(file_ptr, "section .data\n");
    fprintf(file_ptr, "    x db 0\n");
    fprintf(file_ptr, "section .text\n");
    fprintf(file_ptr, "main:\n");
    fprintf(file_ptr, "    sub rsp, 32\n");
}

void codegen_number(NodeReturn node) {
    Number *number = (Number *)node.node;
    fprintf(file_ptr, "    push %d\n", number->value);
}

void codegen_add(NodeReturn node) {
    Number *number = (Number *)node.node;
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    pop rbx\n");
    fprintf(file_ptr, "    add rax, rbx\n");
    fprintf(file_ptr, "    push rax\n");
}

void codegen_mult(NodeReturn node) {
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    pop rdx\n");
    fprintf(file_ptr, "    mul rdx\n");
    fprintf(file_ptr, "    push rax\n");
}

void codegen_var_use(NodeReturn node) {
    UseVar *use_var = (UseVar *)node.node;
    fprintf(file_ptr, "    mov rax, [x+%d]\n", (use_var->index) * 8);
    fprintf(file_ptr, "    push rax\n");
}

void codegen_var(NodeReturn node) {
    VarAssign *var = (VarAssign *)node.node;
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    mov [x+%d], rax\n", (var->index) * 8); //push the value onto the stack
}

void codegen_not_equal(NodeReturn node) {
    BinOp *bin_op = (BinOp *)node.node;    
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    pop rbx\n");
    fprintf(file_ptr, "    cmp rax, rbx\n");
    fprintf(file_ptr, "    jne if_%d\n", bin_op->stack_pos);
    fprintf(file_ptr, "    je end_if_%d\n", bin_op->stack_pos);
}

void codegen_if(NodeReturn node) {
    IfNode *if_node = (IfNode *)node.node;
    fprintf(file_ptr, "if_%d:\n", if_node->stack_pos);
}

void codegen_end_node(NodeReturn node) {
    End *end_node = (End *)node.node;
    fprintf(file_ptr, "end_if_%d:\n", end_node->stack_pos);
}

void codegen_stdout(NodeReturn node) {
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    mov rdi, format\n");
    fprintf(file_ptr, "    mov rsi, rax\n");
    fprintf(file_ptr, "    xor rax, rax\n");
    fprintf(file_ptr, "    call printf\n");
}

void codegen_end() {
    fprintf(file_ptr, "    add rsp, 32\n");
    fprintf(file_ptr, "    ret\n");
    fprintf(file_ptr, "format:\n");
    fprintf(file_ptr, "    db %s, 10, 0", "\"%d\"");   
}