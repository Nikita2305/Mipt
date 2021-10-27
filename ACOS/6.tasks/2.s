    .intel_syntax noprefix
    .text
    .global dot_product

dot_product:
    # rdi, rsi, rdx <=> N, A, B
    mov rax, 0
    vxorps ymm3, ymm3, ymm3
Loop:
    mov rcx, rax
    add rcx, 8
    cmp rcx, rdi
    jg Loopend
    
    vmovups ymm0, [rsi]
    vmovups ymm1, [rdx]
    vmulps ymm2, ymm0, ymm1
    vaddps ymm3, ymm3, ymm2

    add rax, 8
    add rsi, 32
    add rdx, 32
    jmp Loop
Loopend:
    
    vxorps ymm0, ymm0, ymm0
LoopAnother:
    cmp rax, rdi
    je LoopAnotherEnd    
    
    movss xmm1, [rsi]
    movss xmm2, [rdx]
    mulss xmm1, xmm2
    addss xmm0, xmm1

    add rsi, 4
    add rdx, 4
    add rax, 1
    jmp LoopAnother
LoopAnotherEnd:

    vmovaps ymm1, ymm3
    mov rax, 0
LoopAdd:
    vaddps xmm0, xmm0, xmm1
    vpermilps ymm2, ymm1, 0b00111001
    vperm2f128 ymm3, ymm2, ymm2, 0b10000001
    vblendps ymm1, ymm2, ymm3, 0b10001000
    add rax, 1
    cmp rax, 8
    jl LoopAdd
    
    ret
