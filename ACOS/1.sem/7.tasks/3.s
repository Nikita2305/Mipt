#include <sys/syscall.h>
    .intel_syntax noprefix
    .global _start
    .text

_start:
    push rbx
    push rbp
    sub rsp, 16
Loop:
    mov rax, SYS_read
    mov rdi, 0
    mov rsi, rsp
    mov rdx, 1
    syscall
    
    cmp rax, 1
    jne LoopEnd
    
    mov rax, SYS_write
    mov rdi, 1
    mov rsi, rsp
    mov rdx, 1
    syscall
    
    jmp Loop
LoopEnd:
    add rsp, 16
    pop rbp
    pop rbp
    
    mov rax, SYS_exit
    mov rdi, 0
    syscall
