[BITS 32]

global _start

_start:
loop:
    call getkey

    push eax
    mov eax, 3
    int 0x80
    add esp,4
    ; mov eax, 1
    ; push message
    ; int 0x80
    ; add esp, 4
    ; push eax
    ; mov eax, 3
    ; int 0x80
    ; add esp, 4
    jmp loop
getkey:
    mov eax, 2
    int 0x80
    cmp eax, 0x00
    je getkey
    mov [message], al
    ret
    
section .data 
message: db 'This is great, able to talk to kernel from user program!', 0