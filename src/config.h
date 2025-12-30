#ifndef CONFIG_H
#define CONFIG_H

// kernel real mode entry related macros
#define PEACHOS_TOTAL_INTERRUPTS  512
#define CODE_SELECTOR 0x8
#define DATA_SELECTOR 0x10

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
#endif