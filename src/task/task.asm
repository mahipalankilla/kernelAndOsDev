
[BITS 32]

section .asm

global restore_general_puspose_registers
global task_return
global user_registers
; void restore_general_registers(struct registers* regs)
restore_general_puspose_registers:
    push ebp
    mov ebp, esp
    mov ebx, [ebp+8]
    mov edi, [ebx]
    mov esi, [ebx+4]
    mov ebp, [ebx+8]
    mov edx, [ebx+16]
    mov ecx, [ebx+20]
    mov eax, [ebx+24]
    mov ebx, [ebx+12]
    pop ebp
    ret

user_registers:
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret

;void task_return(sruct registers* regs)
task_return:
    mov ebp, esp
    ; push the data segment (SS Will be fine)
    ; push the stack address
    ; push the flags
    ; push the code segment
    ; push the ip
    ; IRET will pop these and we will enter userland


    mov ebx, [ebp+4]

    ; push the data/stack selector
    push dword [ebx+44]
    ; push the stack selector
    push dword [ebx+40]

    pushf
    pop eax
    or eax, 0x200
    push eax

    ; push the code segment
    push dword [ebx+32]

    ; push the ip to execute
    push dword [ebx+28]

    ; setup some segment registers
    mov ax, [ebx+44]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push dword [ebp+4]
    call restore_general_puspose_registers
    add esp, 4

    ; let's leave kernel ring and enter user ring
    iretd


;void user_registers()
