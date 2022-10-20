global main
extern printf
section .text
main:
    sub rsp, 32
    push 10
    pop rax
    mov [rsp], rax
    mov rax, [rsp]
    push rax
    push 1
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    mov [rsp], rax
    mov rax, [rsp]
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