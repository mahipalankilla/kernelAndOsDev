section .asm

global insb
global insw
global outb
global outw

insb:
    push ebp
    mov ebp, esp
    mov DX, [ebp+8]
    IN AL, DX
    pop ebp
    ret
insw:
    push ebp
    mov ebp, esp
    mov DX, [ebp+8]
    IN AX, DX
    pop ebp
    ret
outb:
    push ebp
    mov ebp, esp
    mov DX, [ebp+8]
    mov AL, [ebp+12]
    OUT DX, AL
    pop ebp
    ret

outw:
    push ebp
    mov ebp, esp
    mov DX, [ebp+8]
    mov AX, [ebp+12]
    OUT DX, AX
    pop ebp
    ret




