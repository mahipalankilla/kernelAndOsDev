#ifndef CONFIG_H
#define CONFIG_H

// kernel real mode entry related macros
#define PEACHOS_TOTAL_INTERRUPTS  512
#define KERNEL_CODE_SELECTOR 0x8
#define KERNEL_DATA_SELECTOR 0x10

// Heap implementation macros
#define HEAP_BLOCK_SIZE_BYTES 4096

// we need heap size of 100 MB
#define HEAP_SIZE_IN_BYTES 104857600 // i.e 0x6400000

// hape starting address
#define HEAP_START_ADDR 0x1000000

// Assuming memory at the below address is free and not used for any purpose
// Refer https://wiki.osdev.org/Memory_Map_(x86)
#define HEAP_TABLE_START_ADDR 0x00007E00

#define PEACHOS_SECTOR_SIZE 512

#define PEACHOS_MAX_PATH_LENGTH 108
#define PEACHOS_MAX_FILESYSTEMS 12
#define PEACHOS_MAX_FILE_DESCRIPTORS 512

#define PEACHOS_DESCRIPTOR_SEGMENTS 6
#define PEACHOS_PROGRAM_VIRTUAL_ADDRESS 0x400000

#define PEACHOS_USER_PROGRAM_STACK_SIZE 1024 * 16
#define PEACHOS_PROGRAM_STACK_VIRTUAL_ADDRESS_START 0x3FF000
#define PEACHOS_PROGRAM_STACK_VIRTUAL_ADDRESS_END PEACHOS_PROGRAM_STACK_VIRTUAL_ADDRESS_START - PEACHOS_USER_PROGRAM_STACK_SIZE

// selector consists of 16bit
// 3-15 bits is index into gdt, 2 bit is GDT/LDT, 0-1 Requested previlege level
// Effective previlige level of any code is max(CPL, RPL). This is used to check previlege level set in paging entries
#define USER_DATA_SEGMENT 0x23 // (4 index << 3 | 3 (RPL))
#define USER_CODE_SEGMENT 0x1b // (3 index << 3 | 3 (RPL))

#define PEACHOS_MAX_PROGRAM_ALLOCATIONS 1024 
#define PEACHOS_MAX_PROCESSES 12

#endif