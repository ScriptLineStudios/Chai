global main
extern printf
section .data
    x db 0
section .text
main:
    sub rsp, 32
    push 5
    pop rax
    mov [x+0], rax
    mov rax, [x+0]
    push rax
    push 4
    pop rax
    pop rbx
    cmp rax, rbx
    jne if_0
    je end_if_0
if_0:
    mov rax, [x+0]
    push rax
    push 5
    pop rax
    pop rbx
    cmp rax, rbx
    jne if_1
    je end_if_1
if_1:
    push 70
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
end_if_1:
end_if_0:
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