    .text
    .global solve

solve:
    // x0 = A, w1 = B, w2 = C, w3 = D
    mov x4, 0 // means x
    mov x5, 0 // means increment
Loop:
    add x4, x4, x5
    mov x5, 1
    mul x6, x0, x4 // Ax
    sxtw x7, w1
    add x6, x6, x7 // Ax + B
    mul x6, x6, x4 // Axx + Bx
    sxtw x7, w2
    add x6, x6, x7 // Axx + Bx + C
    mul x6, x6, x4 // Axxx + Bxx + Cx
    sxtw x7, w3
    add x6, x6, x7 // Axxx + Bxx + Cx + D
    cmp x6, 0
    bne Loop
    mov x0, x4
    ret
