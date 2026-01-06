[BITS 32]

global _start

_start:
label:
    push message
    mov eax, 1 ; command print
    int 0x80
    add esp, 4
    jmp $

section .data 
message: db 'This is great, able to talk to kernel from user program!', 0