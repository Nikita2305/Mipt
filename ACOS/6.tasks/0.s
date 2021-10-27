    .intel_syntax noprefix
    .text
    .global very_important_function

very_important_function:
    # rdi, rsi, rdx, rcx <=> N, A, B, C
    mov rax, 0
    vxorps ymm3, ymm3, ymm3
Loop:
    cmp rax, rdi
    je Loopend
    
    vmovups ymm0, [rsi]
    vmovups ymm1, [rdx]
    vaddps ymm2, ymm0, ymm1
    vaddps ymm3, ymm3, ymm2
    vmovups [rcx], ymm2

    add rax, 8
    add rsi, 32
    add rdx, 32
    add rcx, 32
    jmp Loop
Loopend:
    vmovaps ymm1, ymm3
    vxorps ymm0, ymm0, ymm0
    mov rax, 0
LoopAdd:
    vaddps xmm0, xmm0, xmm1
    vpermilps ymm2, ymm1, 0b00111001
    vperm2f128 ymm3, ymm2, ymm2, 0b10000001
    vblendps ymm1, ymm2, ymm3, 0b10001000
    add rax, 1
    cmp rax, 8
    jl LoopAdd
    
    cvtss2sd xmm0, xmm0
    cvtsi2sd xmm1, rdi
    vdivpd ymm0, ymm0, ymm1 
    ret
