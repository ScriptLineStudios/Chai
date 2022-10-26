global main
extern printf
section .text
main:
    sub rsp, 32
    push string_0
    pop rax
    mov [x+0], rax
    push string_1
    pop rax
    mov [x+8], rax
    mov rax, [x+0]
    push rax
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
    mov rax, [x+8]
    push rax
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
    add rsp, 32
    ret
format:
    db "%s", 10, 0
string_0:
    db "strinsg1", 8 , 0

string_1:
    db "strinsg2", 8, 0

section .data
    x db 0
