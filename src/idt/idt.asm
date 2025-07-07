global load_idt
global problem
section .asm

load_idt:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret

problem:    INT 1
    ret