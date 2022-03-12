    .text
    .global main

main:
    sub sp, sp, 16
    str x30, [sp]
L0:
    bl getchar_unlocked
    cmp w0, -1
    beq L1
    cmp w0, 48 
    blt L0
    cmp w0, 57
    bgt L0
    bl putchar_unlocked
    b L0
L1:
    ldr x30, [sp]
    add sp, sp, 16
    ret
