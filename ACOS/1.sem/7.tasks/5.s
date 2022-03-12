#include <sys/syscall.h>
#define size r12
#define total_allocated r13
#define input r14
#define allocation_size 4000

    .intel_syntax noprefix
    .global main
    .data
next_string: .string "\n"
    .text

main:
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15    
 
    mov size, 0
    mov total_allocated, allocation_size
    mov rax, SYS_brk
    mov rdi, 0
    syscall
    mov input, rax

    mov rax, SYS_brk 
    mov rdi, input
    add rdi, total_allocated
    syscall # Allocating memory
    
Loop:
    mov rax, SYS_read
    mov rdi, 0
    mov rsi, input
    add rsi, size
    mov rdx, allocation_size
    syscall # Reading 4k symbols
 
    cmp rax, 0
    je LoopEnd     
 
    add size, rax
    add total_allocated, allocation_size
    mov rax, SYS_brk
    mov rdi, input
    add rdi, total_allocated
    syscall # Allocating inside cycle
     
    jmp Loop
LoopEnd:  

    mov r15, size
    mov rbx, size
    sub rbx, 1

LoopWrite:
    cmp rbx, 0
    jl LoopWriteEnd   
 
    mov rcx, input
    add rcx, rbx
    cmp byte ptr [rcx], 10
    jne BeforeEnd
    
    mov rdi, input
    add rdi, rbx
    add rdi, 1
    mov rsi, r15
    sub rsi, rbx
    sub rsi, 1
    call print_string
    mov r15, rbx

BeforeEnd:
    sub rbx, 1
    jmp LoopWrite

LoopWriteEnd:
    mov rdi, input
    mov rsi, r15
    call print_string
 
    mov rax, SYS_brk
    mov rdi, input
    sub rdi, total_allocated
    syscall # Deallocating 

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx
    
    mov rax, SYS_exit
    mov rdi, 0
    syscall # Exiting

print_string:
    cmp rsi, 0
    jne Write
    ret
Write:
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, 1
    mov rax, SYS_write
    syscall

    mov rax, SYS_write
    mov rdi, 1
    lea rsi, [rip + next_string]
    mov rdx, 1
    syscall
    ret
