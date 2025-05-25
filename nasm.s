section .text
extern sin, cos, sqrt, printf
global main

section .data
    fmt db "%d", 10, 0
    k dq 1
    i dq 0
    n dq 5

main:
    push 5
    pop rax
    mov [n], rax
    mov rax, [n]
    push rax
    call f
    add rsp, 8
    push rax
f:
    push rbp
    mov rbp, rsp
    pop rax
    mov [k], rax
    mov rax, [k]
    push rax
    pop rdi
    mov rsi, rdi
    mov rdi, fmt
    mov rax, 0
    call printf
    mov rax, [k]
    push rax
    pop rbx
    pop rax
    cmp rax, rbx
    sete al
    movzx rax, al
    push rax
    mov rax, 60
    xor rdi, rdi
    syscall
