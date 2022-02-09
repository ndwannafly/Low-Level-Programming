; vim: filetype=nasm

%define stdout 1
%define stderr 2

section .text

global exit
global string_length
global print_char
global print_newline
global print_string
global print_string.with_length
global print_int
global print_uint
global string_equals
global read_char
global parse_uint
global parse_int
global string_copy
global read_word
global out_stderr


; parameters:
;   rdi: exit code
exit:
    mov rax, 60
    syscall
    ; no need for ret

; parameters:
;   rdi: pointer to a zero-terminated string
; returns:
;   rax: string's length
string_length:
    mov rax, -1
.loop:
    inc rax
    cmp byte[rax + rdi], 0 
    jne .loop
    ret

; parameters:
;   rdi: ascii code of the character
print_char:
    mov rsi, rsp
    dec rsi
    mov byte[rsi], dil
    mov rax, 1
    mov rdi, stdout
    mov rdx, 1
    syscall
    ret

print_newline:
    ; answer to question 20
    mov rdi, 10
    jmp print_char

print_string_with_length: ; other function can call this
			  ; if they already has
			  ; string's length stored in rdx
    mov rax, 1
    mov rsi, rdi
    mov rdi, stdout
    syscall
    ret


; parameters:
;   rdi: pointer to a zero-terminated string
print_string:
    call string_length
    mov rdx, rax
    jmp print_string_with_length

out_stderr:
    push rdi
    call string_length
    mov rdx, rax
    mov rax, 1
    pop rsi
    mov rdi, stderr
    syscall
    ret

; parameters:
;   rdi: 64-bit signed integer
print_int:
    ; an attempt to answer question 22
    test rdi, rdi
    jns print_uint
    neg rdi
    push rdi
    mov rdi, '-'
    call print_char
    pop rdi 
; parameters:
;   rdi: 64-bit unsigned integer
print_uint:
    mov rax, rdi
    mov rdi, '0'
    test rax, rax
    jz print_char
    
    mov rdi, rsp
    mov rcx, 10
    
.divide_loop:  
    xor rdx, rdx
    div rcx
    add dl, '0'
    dec rdi
    mov [rdi], dl 
    test rax, rax
    jnz .divide_loop 
    
    mov rdx, rsp
    sub rdx, rdi
    jmp print_string_with_length

; parameters:
;   rdi: pointer to the first zero-terminated string.
;   rsi: pointer to the second zero-terminated string.
; returns:
;   rax = 1 if 2 string are equals, 0 otherwise
string_equals:
.loop:
    mov al, [rdi]
    cmp al, [rsi]
    jne .not_eq
    test al, al 
    jz .eq
    inc rdi
    inc rsi
    jmp .loop
.not_eq:
    xor rax, rax
    ret
.eq:
    mov rax, 1
    ret

; returns:
;   rax: ascii code of the read character
;         0 if there is no more character (EOF)
;        -1 if there is some error
read_char:
    xor rax, rax
    mov rdi, 0
    mov rsi, rsp
    dec rsi
    mov rdx, 1
    syscall
    cmp rax, 0
    jle .end      ; return 0 when no char is read
                  ; or -1 on error
    mov al, [rsi]
.end:
    ret 
    
; parameters:
;   rdi: pointer to a zero-terminated string
; returns:
;   rax: the parsed number
;   rdx: length of the parsed number
parse_uint:
    xor rax, rax
    xor rcx, rcx
.loop:
    cmp byte [rdi], 0
    je .end
    cmp byte [rdi], '0'
    jl .end
    cmp byte [rdi], '9'
    jg .end
    mov rdx, 10
    mul rdx
    xor rdx, rdx 
    mov dl, [rdi]
    add rax, rdx
    sub rax, '0'
    inc rcx
    inc rdi
    jmp .loop
.end:
    mov rdx, rcx
    ret

; parameters:
;   rdi: pointer to a zero-terminated string
; returns:
;   rax: the parsed number
;   rdx: length of the parsed number
parse_int:
    cmp byte [rdi], '-'
    jne parse_uint
    inc rdi
    call parse_uint
    neg rax
    inc rdx
    ret

; parameters:
;   rdi: pointer to the source zero-terminated string
;   rsi: pointer to the destination string's buffer
;   rdx: size of the buffer pointed by rsi
; returns:
;   rax: pointer to the terminated character
;        of the destination string.
;        0 when buffer overflow occurred.
string_copy:
.loop:
    cmp rdx, 0
    je shared_buffer_overflow
    mov al, [rdi]
    mov [rsi], al
    cmp byte[rdi], 0 
    je .done_copy
    dec rdx
    inc rdi
    inc rsi
    jmp .loop
.done_copy: 
    mov rax, rsi
    ret

; parameters:
;   rdi: pointer to the buffer of the result.
;   rsi: the buffer size
; returns:
;   rax: the value of rdi at the invoking moment
;        of this function
;        0 if the buffer was overflowed
read_word:
    push rdi
    push rsi
    push 0
.read_loop: 
    mov rcx, [rsp]
    cmp rcx, [rsp + 1 * 8] 
    jge .buffer_overflow 
    call read_char
    
    ; maybe handling the error case (rax == -1)
    ; differently???
    cmp rax, 0
    jle .got_word       
    
    cmp eax, 9          ; tabulation '\t'
    je .got_word
    cmp eax, 10         ; line break '\n'
    je .got_word
    cmp eax, ' '
    je .got_word
    
    mov rcx, [rsp + 2 * 8]
    add rcx, [rsp]
    mov [rcx], al
    inc qword [rsp] 
    jmp .read_loop
.got_word:
    pop rdx
    pop rax
    pop rax
    mov byte [rax + rdx], 0
    ret
.buffer_overflow:
    add rsp, 3 * 8
    
shared_buffer_overflow:
    xor rax, rax
    ret
