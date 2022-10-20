global main
extern printf
section .text
main:
    sub rsp, 32
    push 1
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
    push 2
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
    add rsp, 32
    ret
format:
    db "%d"