global main
extern printf
section .data
    x db 0
section .text
main:
    sub rsp, 32
    push 3
    pop rax
    mov [x+0], rax
    push 2
    pop rax
    mov [x+8], rax
    mov rax, [x+0]
    push rax
    mov rax, [x+8]
    push rax
    pop rax
    pop rdx
    mul rdx
    push rax
    push 1
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    mov [x+16], rax
    mov rax, [x+16]
    push rax
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
    add rsp, 32
    ret
format:
    db "%d", 10, 0