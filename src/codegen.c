#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

FILE *file_ptr;
void codegen_setup(NodeReturn node) {
    file_ptr = fopen("code.asm", "w");

    fprintf(file_ptr, "global main\n");
    fprintf(file_ptr, "extern printf\n");
    fprintf(file_ptr, "section .text\n");
    fprintf(file_ptr, "main:\n");
    fprintf(file_ptr, "    sub rsp, 32\n");
}

void codegen_number(NodeReturn node) {
    Number *number = (Number *)node.node;
    fprintf(file_ptr, "    push %d\n", number->value);
}

void codegen_var_use(NodeReturn node) {
    fprintf(file_ptr, "    mov rax, [rsp]\n");
    fprintf(file_ptr, "    push rax\n");
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

void codegen_var(NodeReturn node) {
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    mov [rsp], rax\n");
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
    fprintf(file_ptr, "    db %s", "\"%d\"");   
}