    .text
    .global f

f:
    mul x0, x0, x3
    add x0, x0, x1
    mul x0, x0, x3
    add x0, x0, x2
    ret
