section .text
global gdt_load

gdt_load:
    push ebp 
    mov ebp, esp
    mov  eax, [ebp + 8]
    mov [lgdt_descriptor + 2], eax
    mov ax, [ebp + 12]
    mov [lgdt_descriptor], ax
    lgdt[lgdt_descriptor]
    pop ebp
    ret

section .data 
lgdt_descriptor:
    dw 0x0
    dd 0x0