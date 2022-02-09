; vim: filetype=nasm
%include "words.inc"

extern exit
extern read_word
extern print_string_with_length
extern print_newline
extern string_length
extern find_word
extern print_string
extern out_stderr

section .data
msg:
    .input_asker: db "Enter your word (length's limit: 255): ", 0

error_msg:
    .word_overflow: db 27, "[31m", "limit of word is 255 characters!", 27, "[0m", 10, 0
    .word_not_found: db 27, "[31m", "Word's not found", 27, "[0m", 10, 0


%define MAX_LENGTH 256
;input_word: times MAX_LENGTH db 0

section .text
global _start

_start:
    
    mov rdi, msg.input_asker
    call print_string 
    
    sub rsp, MAX_LENGTH
    
    mov rdi, rsp
    mov rsi, MAX_LENGTH
    call read_word
    mov rdi, error_msg.word_overflow
    test rax, rax
    jz exit_error

    mov rdi, rsp
    mov rsi, LIST_POINTER
    call find_word
    mov rdi, error_msg.word_not_found
    test rax, rax
    jz exit_error
    
    mov rdi, rax
    add rdi, COLON_WORD_STR_OFS
    call string_length
    lea rdi, [rdi + rax + 1]
    call print_string
    call print_newline

    mov rdi, 0
    jmp exit


exit_error:
    push rdi
    call out_stderr
    pop rdi

    mov rdi, 1
    jmp exit
