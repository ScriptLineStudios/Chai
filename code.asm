global main
extern printf
section .text
main:
    sub rsp, 32
    push 10
    pop rax
    mov [rsp-0], rax
    push 20
    pop rax
    mov [rsp-8], rax
    mov rax, [rsp-0]
    push rax
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
    add rsp, 32
    ret
format:
    db "%d"