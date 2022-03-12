    .text
    .global sum

sum:
    eor x3, x3, x3
    eor x4, x4, x4
    eor x5, x5, x5
    mov x6, 4
Loop:
    cmp x3, x1
    bge Lend
    mul x5, x3, x6 
    ldr w4, [x2, x5]
    add w0, w0, w4
    add x3, x3, 1
    b Loop
Lend:
    ret
