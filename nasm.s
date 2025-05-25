global main
extern sin, cos, sqrt, printf

section .data
    fmt db "%zu", 10, 0
    n dq 46
    a dq 0
    b dq 1
    c dq 0
    i dq 2
section .text
main:
    call main1
    mov rax, 60
    xor rdi, rdi
    syscall

main1:

    push 46; Number

    pop rax; start Equal
    mov [n], rax; end Equal

    mov rax, [n]; start Identifier
    push rax; end Identifier

    push 2; Number

    pop rbx; start Less
    pop rax
    cmp rax, rbx
    setl al
    movzx rax, al
    push rax; end Less

    pop rax; start If
    test rax, rax
    jz .endif0

    mov rsi, [n]; start Print
    mov rdi, fmt
    xor rax, rax
    call printf; end Print
.endif0:; end If

    push 0; Number

    pop rax; start Equal
    mov [a], rax; end Equal

    push 1; Number

    pop rax; start Equal
    mov [b], rax; end Equal

    push 0; Number

    pop rax; start Equal
    mov [c], rax; end Equal

    push 2; Number

    pop rax; start Equal
    mov [i], rax; end Equal

.while0:; start While

    mov rax, [i]; start Identifier
    push rax; end Identifier

    mov rax, [n]; start Identifier
    push rax; end Identifier

    pop rbx; start LessOrEqual
    pop rax
    cmp rax, rbx
    setle al
    movzx rax, al
    push rax; end LessOrEqual
    pop rax
    test rax, rax
    jz .endwhile0

    mov rax, [a]; start Identifier
    push rax; end Identifier

    mov rax, [b]; start Identifier
    push rax; end Identifier

    pop rbx; start Add
    pop rax
    add rax, rbx
    push rax; end Add

    pop rax; start Equal
    mov [c], rax; end Equal

    mov rax, [b]; start Identifier
    push rax; end Identifier

    pop rax; start Equal
    mov [a], rax; end Equal

    mov rax, [c]; start Identifier
    push rax; end Identifier

    pop rax; start Equal
    mov [b], rax; end Equal

    mov rax, [i]; start Identifier
    push rax; end Identifier

    push 1; Number

    pop rbx; start Add
    pop rax
    add rax, rbx
    push rax; end Add

    pop rax; start Equal
    mov [i], rax; end Equal
    jmp .while0
.endwhile0:; end While

    mov rsi, [b]; start Print
    mov rdi, fmt
    xor rax, rax
    call printf; end Print
    ret
