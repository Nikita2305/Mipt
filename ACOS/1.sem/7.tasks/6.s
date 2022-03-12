    .intel_syntax noprefix
    .global _start
    .data
next_string: .string "\n"
    .text

_start:
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    mov r12, 0 # size 
    mov r13, 4000 # total_allocated
    mov rax, 12 # SYS_brk
    mov rdi, 0
    syscall
    mov r14, rax
    
    mov rax, 12 
    mov rdi, r14
    add rdi, 4000
    syscall # Allocating memory
Loop:
    mov rax, 0 # SYS_read
    mov rdi, 0
    mov rsi, r14
    mov rdx, 1
    syscall
    
    cmp rax, 1
    jne LoopEnd
    
    mov rax, 1 # SYS_write
    mov rdi, 1
    mov rsi, r14
    mov rdx, 1
    syscall
    
    jmp Loop
LoopEnd:
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    
    mov rax, 60 # SYS_exit
    mov rdi, 0
    syscall   


print_string:
    cmp rsi, 0
    jne Write
    ret
Write:
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, 1
    mov rax, 1 # SYS_write
    syscall

    mov rax, 1 # SYS_write
    mov rdi, 1
    lea rsi, [rip + next_string]
    mov rdx, 1
    syscall
    ret
    
syscall:
    // prologue
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15
    // also keep r11 and rcx due to 'syscall' on x64 modifies them
    push r11
    push rcx
    // push arguments into memory to reaarange them
    push r9
    push r8
    push rcx
    push rdx
    push rsi
    push rdi
    // first argument is syscall number, move it into rax
    pop rax
    // move arguments into registers to match syscall calling conventions
    pop rdi
    pop rsi
    pop rdx
    pop r10
    pop r8
    // make actual syscall
    syscall
    // epilogue
    pop rcx
    pop r11
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    ret
