global load_idt
global problem
section .asm
extern int21h_handler
extern no_interrupt_handler

global enable_interrupts
global int21h
global no_interrupt

load_idt:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret

problem:
    pushad
    cli
    INT 32
    popad
    sli
    iret

int21h:
    pushad
    cli
    call int21h_handler
    popad
    sti
    iret

no_interrupt:
    pushad
    cli
    call no_interrupt_handler
    popad
    sti
    iret

enable_interrupts:
    push ebp    
    mov ebp, esp
    sti
    pop ebp
    ret