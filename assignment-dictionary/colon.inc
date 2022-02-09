; vim: filetype=nasm

%xdefine LIST_POINTER 0
%define COLON_WORD_STR_OFS 8

%macro colon 2
%%LIST_POINTER:
    dq LIST_POINTER
    db %1, 0
 %2:
    %xdefine LIST_POINTER %%LIST_POINTER
%endmacro
