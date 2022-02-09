; vim: filetype=nasm

section .text
 
 
; description: Accepts an exit code and terminates current process
; parameters:
;	rdi: exit code
exit: 
    xor rax, 60
    syscall
    ; no need for ret


; description: Accepts a pointer to a string and returns its length
; parameters:
;	rdi: pointer to a zero-terminated string
; returns:
;	rax: string length
string_length:
    mov rax, -1
.loop:
    inc rax
    cmp byte[rdi + rax], 0
    jne .loop
    ret

; description: Accepts a pointer to a null-terminated string and prints it to stdout
; parameters:
;	rdi: pointer to a zero-terminated string
print_string:
    push rdi 
    call string_length
    pop rdi
    mov rdx, rax
.with_length:	; other function can call this
		; if they already has
		; string length stored in rdx
	
    mov rax, 1
    mov rsi, rdi
    mov rdi, 1
    syscall
    ret

; description: Accepts a character code directly as its first argument and prints it to stdout.
; parameters:
;	rdi: ascii code of the character
print_char:
    mov rsi, rsp
    dec rsi
    mov byte[rsi], dil ; dil is used to get  8 smallest bits of rdi.
    		       ; ASCII characters are represented by 8 bits.
    mov rax, 1
    mov rdi, 1
    mov rdx, 1
    syscall
    ret

; Переводит строку (выводит символ с кодом 0xA)
print_newline:
    xor rax, rax
    ret




; description: Output a signed 8-byte integer in decimal format
; parameters:
;        rdi: 64-bit signed integer
print_int:
    test rdi, rdi
    jns print_uint
    neg rdi
    push rdi
    mov rdi, '-'
    call print_char
    pop rdi
    jns print_uint
    
; description: Outputs an unsigned 8-byte integer in decimal format. We suggest you create a buffer
;	       on the stack 6 and store the division results there. Each time you divide the last value
;	       by 10 and store the corresponding digit inside the buffer.
;	       Do not forget, that you should transform each digit into its ASCII code (e.g., 0x04 becomes 0x34).
; parameters:
;	rdi: 64-bit unsigned integer
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
    mov[rdi], dl
    test rax, rax
    jnz .divide_loop

    mov rdx, rsp
    sub rdx, rdi
    jmp print_string.with_length


; Принимает два указателя на нуль-терминированные строки, возвращает 1 если они равны, 0 иначе
; (eng)
; description: Accepts two pointers to strings and compares them. Returns 1 if they are
; equal, otherwise 0.
; parameters:
;    rdi: pointer to the first zero-terminated string
;    rsi: pointer to the second zero-terminated string
;    rax = 1 if 2 string are equals, 0 otherwise
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

; description: Read one character from stdin and return it. if the end of input stram occurs, return 0
; return:
;     rax: ascii code of the read character
;	     0 if there is no more character (EOF - end of file)
; 	     -1 if there is some error
read_char:
    xor rax, rax
    mov rdi, 0
    mov rsi, rsp
    dec rsi
    mov rdx, 1
    syscall
    cmp rax, 0
    jle .end	; return 0 when no char is read
    		; return -1 on error
    mov al, [rsi] 	; copies 8 sequential bytes starting at address, 
    			; stored in rsi , into al (8 smallest bits of rax)
.end:
    ret

; description: Accepts a buffer address and size as arguments. Reads next word from stdin
;		(skipping white spaces 7 into buffer). Stops and returns 0 if word is too
;		big for the buffer specified: otherwise returns a buffer address. This
;		function should null-terminate the accepted string.

; parameters:
;     rdi: pointer to the buffer of the result
;     rsi: the buffer size
; returns:
;   rax: the value of rdi at the invoking moment of this function
;        0 if the buffer was overflowed

; wrong testcase "\t   123"
read_word:
    push rdi
    push rsi
    push 0
.skip_spaces:
    mov rcx, [rsp]
    cmp rcx, [rsp + 1 * 8]
    jge .buffer_overflow
    call read_char
    
    ; handling the error case (rax == -1)
    cmp rax, 0
    jle .got_word

    cmp al, 9		; tabulation '\t'
    je .skip_spaces
    
    cmp al, 10		; line breal '\n'
    je .skip_spaces

    cmp al, ' '
    je .skip_spaces
    
    mov rcx, [rsp + 2 * 8]
    add rcx, [rsp]
    mov [rcx], al
    inc qword [rsp]  

.read_loop:
    mov rcx, [rsp]
    cmp rcx, [rsp + 1 * 8]
    jge .buffer_overflow
    call read_char

    cmp rax, 0
    jle .got_word

    cmp al, 9		; tabulation '\t'
    je .got_word
    cmp al, 10		; line break '\n'
    je .got_word	
    cmp al, ' '
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
    jmp buffer_overflow

 

; Принимает указатель на строку, пытается
; прочитать из её начала беззнаковое число.
; Возвращает в rax: число, rdx : его длину в символах
; rdx = 0 если число прочитать не удалось
; (eng)
; description: Outputs an unsigned 8-byte integer in decimal format. We suggest
;	       you create a buffer on the stack 6 and store the division results
;   	       there. Each time you divide the last value by 10 and store the
;	       corresponding digit inside the buffer. Do not forget, that you
;	       should transform each digit into its ASCII code
;	       (e.g. 0x04 becomes 0x34).
; parameters:
;    rdi: 64-bit unsigned integer
parse_uint:
    xor rax, rax
    xor rcx, rcx
.loop:
    cmp byte [rdi], 0
    je .end
    cmp byte [rdi], '0'
    jl .end
    cmp byte[rdi], '9'
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




; Принимает указатель на строку, пытается
; прочитать из её начала знаковое число.
; Если есть знак, пробелы между ним и числом не разрешены.
; Возвращает в rax: число, rdx : его длину в символах (включая знак, если он был) 
; rdx = 0 если число прочитать не удалось
; (eng)
; description: Accepts a null-terminated strig and tries to parse a signed number
; 	       from its start. Returns the number parsed in rax; it characters
;	       count in rdx (including sign if any). No spaces between sign and
;	       digits are allowed
; parameters:
;    rdi: pointer to a zero-terminated string
; returns:
;    rax: the parsed number
;    rdx: length of the parsed number
parse_int:
    cmp byte[rdi], '-'
    jne parse_uint
    inc rdi
    call parse_uint
    neg rax
    inc rdx
    ret 

; Принимает указатель на строку, указатель на буфер и длину буфера
; Копирует строку в буфер
; Возвращает длину строки если она умещается в буфер, иначе 0

string_copy:
.loop:
    cmp rdx, 0
    je buffer_overflow
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

buffer_overflow:
    xor rax, rax
    ret



