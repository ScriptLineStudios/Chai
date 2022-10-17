#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "lexer.h"

FILE *file_ptr;
void codegen_setup(NodeReturn node) {
    file_ptr = fopen("code.asm", "w");
    fprintf(file_ptr, "section .text\n");
    fprintf(file_ptr, "    global _start\n");
    fprintf(file_ptr, "_start:\n");
    fprintf(file_ptr, "    mov rax, 0\n");
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

void codegen_var(NodeReturn node) {
    fprintf(file_ptr, "    pop rax\n");
    fprintf(file_ptr, "    mov [rsp], rax\n");
}

void codegen_end() {
    fprintf(file_ptr, "    mov rax, 60\n");
    fprintf(file_ptr, "    mov rdi, 0\n");
    fprintf(file_ptr, "    syscall\n");
}