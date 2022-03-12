    .data
    .global R
R:  .space 8
    
    .text
    .extern A, B, C, D
    .global calculate

calculate:
    ldr x0, A
    ldr x1, B
    mul x0, x0, x1
    ldr x1, C
    ldr x2, D
    mul x1, x1, x2
    add x0, x0, x1
    adr x1, R
    str x0, [x1] 
    ret
