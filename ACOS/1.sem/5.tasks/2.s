.intel_syntax noprefix
    .text
    .global sum

sum:
    sub rsp, 8
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15
    # rdi, rsi, rdx, rcx <=> N, A, B, C 
    mov rax, 0 # index 
Loop:
    cmp rax, rdi
    je Loopend
    
    mov ebx, [rsi + 4 * rax]
    add ebx, [rdx + 4 * rax]
    mov [rcx + 4 * rax], ebx

    add rax, 1
    jmp Loop
Loopend:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    add rsp, 8
    ret
