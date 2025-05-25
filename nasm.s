global main
extern sin, cos, sqrt, printf

section .data
    fmt db "%d", 10, 0
    n dq 12
    k dq 1
    i dq 0
    z dq 0
section .text
main:
    call main1
    mov rax, 60
    xor rdi, rdi
    syscall

main1:

    push 12; Number

    pop rax; start Equal
    mov [n], rax; end Equal

    push 1; Number

    pop rax; start Equal
    mov [k], rax; end Equal

    push 0; Number

    pop rax; start Equal
    mov [i], rax; end Equal

.while0:; start While

    mov rax, [i]; start Identifier
    push rax; end Identifier

    mov rax, [n]; start Identifier
    push rax; end Identifier

    pop rbx; start Less
    pop rax
    cmp rax, rbx
    setl al
    movzx rax, al
    push rax; end Less
    pop rax
    test rax, rax
    jz .endwhile0

    mov rax, [i]; start Identifier
    push rax; end Identifier

    push 1; Number

    pop rbx; start Add
    pop rax
    add rax, rbx
    push rax; end Add

    pop rax; start Equal
    mov [i], rax; end Equal

    mov rax, [k]; start Identifier
    push rax; end Identifier

    mov rax, [i]; start Identifier
    push rax; end Identifier

    pop rbx; start Mul
    pop rax
    imul rax, rbx
    push rax; end Mul

    pop rax; start Equal
    mov [k], rax; end Equal

    mov rax, [n]; start Identifier
    push rax; end Identifier

    mov rax, [i]; start Identifier
    push rax; end Identifier

    pop rbx; start Identical
    pop rax
    cmp rax, rbx
    sete al
    movzx rax, al
    push rax; end Identical

    pop rax; start If
    test rax, rax
    jz .endif0

    push 0; Number

    pop rax; start Equal
    mov [z], rax; end Equal

    mov rax, [k]; start Identifier
    push rax; end Identifier

    pop rax; start Equal
    mov [z], rax; end Equal
.endif0:; end If
    jmp .while0
.endwhile0:; end While
    ret
