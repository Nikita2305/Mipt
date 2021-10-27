    .intel_syntax noprefix
    .text
    .global mergesort

mergesort:
    # from, to, in, out - rdi, rsi, rdx, rcx 
    sub rsp, 8 
    push rbx
    push rbp     

    mov r8, 0
    mov r9, rsi
    sub r9, rdi # r9 = segment size

    mov rbx, r9
    mov rax, 16
    imul rbx, rax # stack array size

    mov rbp, rsp
    sub rsp, rbx # now [rbp - 16 * x - 16] is arr[x]
    call msort
    add rsp, rbx

    pop rbp
    pop rbx
    add rsp, 8
    ret

msort:
    # from_in - rdi
    # to_in - rsi
    # in - rdx
    # out - rcx
    # from_out - r8
    # to_out - r9
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov rax, rsi
    sub rax, rdi
    cmp rax, 1
    jne Lsort

    # base
    mov eax, [rdx + 4 * rdi]
    mov [rcx + 4 * r8], eax 
    jmp Lend # return

Lsort:
    # Call Left 
    push rdi
    push rsi
    push rdx
    push rcx    
    push r8
    push r9

    mov rax, rdi
    add rax, rsi
    sar rax, 1
    mov rsi, rax 

    mov rax, r8
    add rax, r9
    sar rax, 1
    mov r9, rax
    
    call msort

    pop r9
    pop r8
    pop rcx
    pop rdx    
    pop rsi
    pop rdi

    # Call Right
    push rdi
    push rsi
    push rdx
    push rcx    
    push r8
    push r9

    mov rax, rdi
    add rax, rsi
    sar rax, 1
    mov rdi, rax 

    mov rax, r8
    add rax, r9
    sar rax, 1
    mov r8, rax
    
    call msort

    pop r9
    pop r8
    pop rcx
    pop rdx    
    pop rsi
    pop rdi

    # Merge
    mov rax, r8
    add rax, r9
    sar rax, 1 # [r8 .... rax - 1, rax ... r9) # output
    
    mov r10, r8 # Lptr
    mov r11, rax # Rptr
    mov r12, r8 # OUTptr
    
Loop:
    cmp r12, r9
    je Lcopy
    
    cmp r11, r9
    je Lleft
    cmp r10, rax
    je Lright
    jmp Lchoose
    
Lleft:
    mov ebx, [rcx + 4 * r10]
    add r10, 1
    jmp Linsert

Lright:
    mov ebx, [rcx + 4 * r11]
    add r11, 1
    jmp Linsert

Lchoose:
    mov ebx, [rcx + 4 * r10]
    cmp ebx, [rcx + 4 * r11]
    jle Lleft
    jmp Lright

Linsert:
    mov r13, r12
    mov r14, 16
    imul r13, r14
    mov r14, rbp
    sub r14, r13
    sub r14, 16
    mov [r14], ebx # [rbp - 16 * r12 - 16]
 
    add r12, 1
    jmp Loop
Lcopy:
    mov r10, r8 # COPYptr
Lcloop:
    cmp r10, r9
    je Lend
    
    mov r13, r10
    mov r14, 16
    imul r13, r14
    mov r14, rbp
    sub r14, r13
    sub r14, 16
    mov ebx, [r14] # [rbp - 16 * r10 - 16]
    
    mov [rcx + 4 * r10], ebx
    add r10, 1
    jmp Lcloop  
Lend:
    pop r15
    pop r14
    pop r13
    pop r12 
    pop rbx
    ret

.data
printf_format:
    .string "%d %d %d %d\n"
