; vim: filetype=nasm
%include "colon.inc"

extern string_equals
global find_word

section .text

; paramters:
;   rdi: pointer to null-terminated string
;        to the searching word.
;   rsi: pointer to the linked list of the dictionary
; returns:
;   rax: 0 if word not found
;        pointer tothe word's record if found
find_word:
    push rbx
    push rbp
    mov rbp, rdi
    mov rbx, rsi
.loop:    
    test rbx, rbx
    jz .done
    mov rdi, rbp
    lea rsi, [rbx + COLON_WORD_STR_OFS]
    call string_equals
    test rax, rax
    jnz .done
    mov rbx, [rbx]
    jmp .loop
    
.done:
    mov rax, rbx
    pop rbp
    pop rbx
    ret
    

