    .data
scanf_format: .string "%d %d"
printf_format: .string "%d\n"

    .text
    .global main

main:
    sub sp, sp, 16
    str x30, [sp]

    adr x0, scanf_format
    sub sp, sp, 16
    mov x1, sp
    sub sp, sp, 16
    mov x2, sp
    bl scanf
 
    adr x0, printf_format
    ldr w2, [sp]
    ldr w1, [sp, 16]
    add w1, w1, w2 
    bl printf
    
    add sp, sp, 16
    add sp, sp, 16
    ldr x30, [sp]
    add sp, sp, 16
    ret
