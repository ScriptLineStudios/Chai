global main
extern printf
section .data
    x db 0
section .text
main:
    sub rsp, 32
    push 10
    push 4
    add rsp, 32
    ret
format:
    db "%d", 10, 0