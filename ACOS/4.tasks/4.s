    .data
printf_format: .string "%d\n"
    .text
    .global main

main:
    // x19 - char*
    // x20 - capacity
    // x21 - size
    sub sp, sp, 16
    str x30, [sp]
    sub sp, sp, 16
    str x19, [sp]
    sub sp, sp, 16
    str x20, [sp]
    sub sp, sp, 16
    str x21, [sp]
    //initializing
    mov x20, 1
    mov x21, 0
    mov x0, x20
    lsl x0, x0, 2
    bl malloc
    mov x19, x0 
Loop: 
    cmp x21, x20
    bge Lrealloc
    bl getchar_unlocked
    cmp w0, -1
    beq Lend
    str w0, [x19, x21, lsl 2]
    add x21, x21, 1
    b Loop
Lend:
    // print
    sub x21, x21, 1
Lprint:
    cmp x21, 0
    blt Lfree
    ldr w0, [x19, x21, lsl 2]
    bl putchar_unlocked
    sub x21, x21, 1
    b Lprint
Lfree:
    // free
    mov x0, x19
    bl free
    ldr x21, [sp]
    add sp, sp, 16
    ldr x20, [sp]
    add sp, sp, 16
    ldr x19, [sp]
    add sp, sp, 16
    ldr x30, [sp]
    add sp, sp, 16
    ret

Lrealloc: // TODO: test
    lsl x0, x20, 1
    add x0, x0, 1
    lsl x0, x0, 2
    bl malloc
    mov x1, 0 //i
LocalLoop:
    cmp x1, x21
    bge LocalLoopEnd
    ldr w2, [x19, x1, lsl 2]
    str w2, [x0, x1, lsl 2]
    add x1, x1, 1
    b LocalLoop
LocalLoopEnd: 
    lsl x20, x20, 1
    add x20, x20, 1
    mov x1, x19
    mov x19, x0
    mov x0, x1 
    bl free 
    b Loop
