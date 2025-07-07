section .txt
global _start
ORG 0x7c00
BITS 16

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
_start:
	jmp short start
	nop
times 33 db 0

start:
	jmp 0:step2

step2:
	cli
	mov ax, 0x00
	mov ds, ax
	mov es, ax
	mov ax, 0x00
	mov ss, ax
	mov sp, 0x7c00
	sti	

.load_protected:
	cli
	lgdt[gdt_descriptor]
	mov eax, cr0
	or eax, 0x1
	mov cr0, eax ; switch on protected mode
	jmp CODE_SEG:load32

;GDT
gdt_start:
gdt_null:
	dd 0x0
	dd 0x0

;offset 0x8
gdt_code:
	dw 0xffff ; segment limit first 0-15 bits
	dw 0 ; base first 0-15 bits
	db 0 ; base 16-23 bits
 	db 0x9a ; Acess byte
	db 11001111b ; High 4 bit flags and low 4 bit flags
	db 0 ; base 24-31 bits

; offset 0x10
gdt_data:
	dw 0xffff ; segment limit first 0-15 bits
	dw 0 ; base first 0-15 bits
	db 0 ; base 16-23 bits
 	db 0x92 ; Acess byte
	db 11001111b ; High 4 bit flags and low 4 bit flags
	db 0 ; base 24-31 bits

gdt_end:

gdt_descriptor:
	dw gdt_end - gdt_start - 1
	dd gdt_start

[BITS 32]
load32:
	mov eax, 1 ; using eax to store lba
	mov ecx, 100 ; number of sectors to read
	mov edi, 0x0100000 ; address to which kernel is read
	call ata_lba_read
	jmp CODE_SEG:0x0100000

ata_lba_read:
	mov ebx, eax ; backup lba in ebx

	; send bit 24-27 of lba with lba mode on
	mov edx, 0x01F6
	shr eax, 24
	or al, 11100000b ; set bit 6 in al for lba mode
	out dx, al
	; complete sending bits 24-27

	; send number of sectors
	mov edx, 0x01F2
	mov al, cl ; number of sectors to read
	out dx, al

	; send bit 0-7 of LBA
	mov edx, 0x1F3
	mov eax, ebx
	out dx, al

	; send bit 8-15 of lba, eax is 32 bit, ax is lower 16 bits (al lower byte, ah higher byte)
	mov edx, 0x1F4
	mov eax, ebx
	shr eax, 8
	out dx, al

	; send bit 16-23 of LBA
	mov edx, 0x1F5
	mov eax, ebx
	shr eax, 16
	out dx, al
	
	; command port
	mov edx, 0x1F7
	mov al, 0x20 ; write with retry, check what are bit values at port 0x1F7 port
	out dx, al

.next_sector:
	push ecx

.in_progress:
	mov dx, 0x1F7
	in ax, dx
	test al, 8 ; check the fields of input from port 0x1F7
	jz .in_progress ; until the sector buffer is ready

; we need to read 256 words at a time
	mov ecx, 256
	mov edx, 0x1F0
	rep insw
	pop ecx
	loop .next_sector
	; End of reading sectors into memory
	ret

times 510-($-$$) db 0
dw 0xAA55