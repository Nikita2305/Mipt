    .intel_syntax noprefix
    .text
    .global avx_sin

avx_sin:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15
    #                       xmm0 = x
    movsd xmm1, xmm0
    movsd xmm3, twopi[rip]
    vdivpd xmm2, xmm1, xmm3
    cvtsd2si rdi, xmm2
    cvtsi2sd xmm2, rdi
    vmulpd xmm2, xmm2, xmm3
    vsubpd xmm0, xmm0, xmm2

    #           ymm1 = (-xxxxxxx / 7!, xxxxx / 5!, -xxx / 3!, x / 1)
    movsd xmm1, xmm0
    vmulpd xmm1, xmm1, xmm0 # = xx
    vmulpd xmm1, xmm1, xmm0 # = xxx    

    vmulpd xmm2, xmm1, xmm0 # = xxxx
    vmulpd xmm2, xmm2, xmm0 # = xxxxx

    vmulpd xmm3, xmm2, xmm0 # = xxxxxx
    vmulpd xmm3, xmm3, xmm0 # = xxxxxxx
    
    mov rdi, -6
    cvtsi2sd xmm4, rdi
    vdivpd xmm1, xmm1, xmm4
    
    mov rdi, 120
    cvtsi2sd xmm4, rdi
    vdivpd xmm2, xmm2, xmm4

    mov rdi, -5040
    cvtsi2sd xmm4, rdi
    vdivpd xmm3, xmm3, xmm4
    
    vunpcklpd xmm1, xmm0, xmm1
    vunpcklpd xmm2, xmm2, xmm3
    vinsertf128 ymm1, ymm1, xmm2, 0b00000001
    
    # ymm0, ymm1 is locked
    #                           ymm3 = (8, 6, 4, 2)
    mov rdi, 2
    cvtsi2sd xmm2, rdi
    mov rdi, 4
    cvtsi2sd xmm3, rdi
    mov rdi, 6
    cvtsi2sd xmm4, rdi
    mov rdi, 8
    cvtsi2sd xmm5, rdi
    vunpcklpd xmm2, xmm2, xmm3
    vunpcklpd xmm4, xmm4, xmm5
    vinsertf128 ymm3, ymm2, xmm4, 0b00000001
    
    # ymm0, ymm1, ymm3 is locked
    #                           ymm2 = (x^8, x^8, x^8, x^8)
    movsd xmm4, xmm0
    mulsd xmm4, xmm4 # xx
    mulsd xmm4, xmm4 # xxxx
    mulsd xmm4, xmm4 # xxxxxxxx
    vpermilpd ymm5, ymm4, 0b00000000      # [..., ..., x, x]
    vperm2f128 ymm2, ymm5, ymm5, 0b00000000 # [x, x, x, x]

    # ymm0, ymm1, ymm2, ymm3 is locked
    #                           ymm4 = (1, 1, 1, 1)
    mov rdi, 1
    cvtsi2sd xmm5, rdi
    vpermilpd ymm6, ymm5, 0b00000000      # [..., ..., 1, 1]
    vperm2f128 ymm4, ymm6, ymm6, 0b00000000 # [1, 1, 1, 1]

    mov rcx, 1
    vxorps ymm5, ymm5, ymm5 # sum
    
Loop:
    vaddpd ymm5, ymm5, ymm1
    
    # Mul factorials
    mov rdx, 0
LoopNested:
    vdivpd ymm1, ymm1, ymm3
    vaddpd ymm3, ymm3, ymm4
    add rdx, 1
    cmp rdx, 8  
    jne LoopNested
    
    # Mul x^8
    vmulpd ymm1, ymm1, ymm2    

    add rcx, 1    
    cmp rcx, 100000
    jne Loop
    
    # ymm5 = sum
    vxorps ymm0, ymm0, ymm0
    mov rax, 0
LoopAdd:
    vaddpd xmm0, xmm0, xmm5
    vpermilpd ymm1, ymm5, 0b00000101        # [3, 4, 1, 2]
    vperm2f128 ymm2, ymm1, ymm1, 0b10000001 # [0, 0, 3, 4]
    vblendpd ymm5, ymm1, ymm2, 0b00001010   # [0, 4, 3, 2]
    add rax, 1
    cmp rax, 4
    jne LoopAdd
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
.data
.align 16
    twopi:
        .double 6.283185307179586 
