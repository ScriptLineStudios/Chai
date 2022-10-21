global main
extern printf
section .data
    x db 0
section .text
main:
    sub rsp, 32
    push 100
    pop rax
    mov [x+0], rax
    mov rax, [x+0]
    push rax
    push 4
    pop rax
    pop rbx
    cmp rax, rbx
    jne if
    je end_if
if:
    mov rax, [x+0]
    push rax
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
end_if:
    push 7
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
    add rsp, 32
    ret
format:
    db "%d", 10, 0