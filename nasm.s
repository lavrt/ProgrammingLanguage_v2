section .text
extern sin, cos, sqrt, printf
global _start

section .data
    fmt db "733343648", 10, 0
    k dq 1
    i dq 0
    n dq 5

_start:
    call main
    mov rax, 60
    xor rdi, rdi
    syscall

main:
    push 5
    pop rax    mov [    mov [%s], rax
], rax
    mov rax, [n]
    push rax
    call f
    add rsp, 8
    push rax
    pop rax    mov [    mov [%s], rax
], rax
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
    ret
