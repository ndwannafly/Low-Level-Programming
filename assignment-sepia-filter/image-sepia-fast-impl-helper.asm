; vim: filetype=nasm

section .text
global fast_sepia_inplace_helper

; rdi: the data address
; rsi: the number of pixels to be processed
; rdx: the transposed coefficient matrix
fast_sepia_inplace_helper:
    pxor xmm6, xmm6
    xor rcx, rcx
.loop:
    cmp rsi, rcx
    jle .end
    lea rcx, [rcx + 4]
%macro process_component 5
    %define RESULT_REG %1
    %define COMPONENT_OFFSET %2
    %define COMPONENT_ID %3
    %define COMPONENT_MASK %4
    %define COEFFICIENT_MASK %5

    movd xmm0, [rdi + COMPONENT_OFFSET + COMPONENT_ID]
    punpcklbw xmm0, xmm6
    punpcklwd xmm0, xmm6
    pshufd xmm0, xmm0, COMPONENT_MASK
    cvtdq2ps xmm0, xmm0

    movdqu xmm1, [rdx + COMPONENT_ID * 3 * 4]
    pshufd xmm1, xmm1, COEFFICIENT_MASK
    mulps xmm1, xmm0
    addps RESULT_REG, xmm1
%endmacro

%macro process_group 4
    %define RESULT_REG %1
    %define COMPONENT_OFFSET %2
    %define COMPONENT_MASK %3
    %define COEFFICIENT_MASK %4
    pxor RESULT_REG, RESULT_REG
    process_component RESULT_REG, COMPONENT_OFFSET, 0, COMPONENT_MASK, COEFFICIENT_MASK
    process_component RESULT_REG, COMPONENT_OFFSET, 1, COMPONENT_MASK, COEFFICIENT_MASK
    process_component RESULT_REG, COMPONENT_OFFSET, 2, COMPONENT_MASK, COEFFICIENT_MASK
    cvttps2dq RESULT_REG, RESULT_REG
    packusdw RESULT_REG, xmm6
    packuswb RESULT_REG, xmm6
%endmacro

    process_group xmm2, 0, 0b11_00_00_00, 0b00_10_01_00
    movd eax, xmm2
    process_group xmm2, 3, 0b11_11_00_00, 0b01_00_10_01
    mov [rdi], eax
    movd eax, xmm2
    process_group xmm2, 6, 0b11_11_11_00, 0b10_01_00_10
    mov [rdi + 4], eax
    movd eax, xmm2
    mov [rdi + 8], eax
    lea rdi, [rdi + 12]
    jmp .loop
.end:
    ret
