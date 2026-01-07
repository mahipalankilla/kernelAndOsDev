
section .asm

global load_idt
global problem

extern isr80h_handler
global enable_interrupts
global isr80h_wrapper
extern interrupt_handler

load_idt:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    lidt [ebx]
    pop ebp
    ret

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

%macro interrupt 1
    global int%1
    int%1:
        cli
        pushad
        push esp 
        push dword %1
        call interrupt_handler
        add esp, 8
        popad
        iret
%endmacro

%assign i 0
%rep 512
    interrupt i
    %assign i i+1
%endrep

section .data

global interrupt_func_addresses

%macro interrupt_address 1
    dd int%1
%endmacro

interrupt_func_addresses:
%assign i 0
%rep 512
    interrupt_address i
    %assign i i+1
%endrep
temp_res: dw 0x0
