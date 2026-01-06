global load_idt
global problem
section .asm
extern int21h_handler
extern no_interrupt_handler
extern isr80h_handler
global enable_interrupts
global int21h
global no_interrupt
global isr80h_wrapper

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

; when interrupt is called and there is a change of previlege level, the following are pushsed into stack of the previlage level in which is
; push ss
; push sp
; push eflags
; push cs
; push ip

; for INT to execute, CPL of code executing INT should be <= DPL set in IDT entry for the interrupt
; for far call/jump into another segement code from one segment, CPL >= DPL
; EPL only comes into picture only  during segment register loading and page access checks during paging
isr80h_wrapper:
    cli
    ; pushes general purpose registers to the stack
    pushad
    ; Interrupt frame end

    ; push the stack pointer so that we are pointing to the interrupt frame
    push esp

    ; push command that's passed in eax by user function
    push eax
    call isr80h_handler
    mov dword[temp_res], eax
    add esp, 8

    ; restore general purpose registers for user land
    popad
    mov eax, [temp_res]
    iretd


section .data
temp_res: dw 0x0

