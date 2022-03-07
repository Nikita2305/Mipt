    .text
    .global sum

sum:
    orr x3, x3, x3 // element value
    orr x4, x4, x4 // i
    orr x5, x5, x5 // element ptr
    mov x6, 4 // special number
Loop:
    cmp x4, x1
    bge Lend
    mul x5, x4, x6 
    ldr w3, [x2, x5]
    add w0, w0, w3
    add x4, x4, 1
    b Loop
Lend:
    ret

