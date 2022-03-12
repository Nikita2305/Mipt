    .intel_syntax noprefix
    .text
    .global very_important_function
    .extern A

very_important_function:
    sub rsp, 8
    sub rsp, 16
    mov [rsp], rdi # Size

    sub rsp, 16
    mov rdi, 0
    mov [rsp], rdi # index
Loop:
    mov rdi, [rsp] # index
    mov rsi, [rsp + 16] # size
    cmp rdi, rsi
    je Loopend # index == size
    sub rsp, 16 # For scanned element

    lea rdi, [rip + scanf_format] 
    mov rsi, rsp
    call scanf

    lea rdi, [rip + printf_format]
    movsxd rsi, [rsp] # scanned value
    mov rdx, [rsp + 16] # index
    mov rax, A[rip]
    movsxd rcx, [rax + 4 * rdx] # array value
    imul rsi, rcx
    call printf

    add rsp, 16
    mov rsi, [rsp]
    add rsi, 1
    mov [rsp], rsi
    jmp Loop
Loopend:
    add rsp, 32
    add rsp, 8
    ret    

    .data
printf_format:
    .string "%lld\n"
scanf_format:
    .string "%d"
