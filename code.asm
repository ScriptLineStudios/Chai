global main
extern printf
section .text
main:
    push 6
    push 10000
    push 7
    pop rax
    pop rdx
    mul rdx
    push rax
    pop rax
    pop rbx
    add rax, rbx
    push rax
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
    ret
format:
    db "%d"