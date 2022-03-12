    .data
Hello: .string "Hello\n"
    
    .text
    .global main

main:
    sub sp, sp, 16
    str x30, [sp]
    adr x0, Hello
    bl printf
    ldr x30, [sp]
    add sp, sp, 16
    ret
