global main
extern printf
section .data
    x db 0
section .text
main:
    sub rsp, 32
    push 41
    pop rax
    mov [x+0], rax
    mov rax, [x+0]
    push rax
    push 4
    push 10
    pop rax
    pop rdx
    mul rdx
    push rax
    pop rax
    pop rbx
    cmp rax, rbx
    jne if_0
    je end_if_0
if_0:
    push 5
    pop rax
    mov [x+8], rax
    mov rax, [x+0]
    push rax
    push 5
    pop rax
    pop rbx
    cmp rax, rbx
    jne if_1
    je end_if_1
if_1:
    mov rax, [x+8]
    push rax
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
end_if_1:
    push 70
    pop rax
    mov rdi, format
    mov rsi, rax
    xor rax, rax
    call printf
end_if_0:
    add rsp, 32
    ret
format:
    db "%d", 10, 0