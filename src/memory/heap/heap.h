#ifndef HEAP_H
#define HEAP_H

#include "config.h"
#include <stdint.h>
#include <stddef.h>

#define HEAP_BLOCK_TABLE_ENTRY_TAKEN 0x01
#define HEAP_BLOCK_TABLE_ENTRY_FREE 0x00

#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_LAST  0b01000000

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;


typedef struct
{
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t total;
}heapTable;

typedef struct
{
    heapTable table;

    // start address of the heap pool
    void* saddr;
}heap;

int CreateHeap(heap* heap, uint32_t heapTableStAddr, void* startAddr, void* endAddr);

// allocate required number of bytes in multiple of HEAP_BLOCK_SIZE_BYTES and return start address of memory allocated
void* HeapMalloc(heap* heap, size_t bytes);

int HeapFree(heap* heap, void* addr);

#endif