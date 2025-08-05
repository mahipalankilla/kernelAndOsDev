#include "memory/heap/kheap.h"
#include "memory/heap/heap.h"

heap kernelHeap;

void KheapInit(void)
{
    CreateHeap(&kernelHeap, HEAP_TABLE_START_ADDR, (void*)HEAP_START_ADDR, (void*)(HEAP_START_ADDR + HEAP_SIZE_IN_BYTES - 1));
}

void* kMalloc(size_t bytes)
{
    return HeapMalloc(&kernelHeap, bytes);
}

void kFree(void* addr)
{
    HeapFree(&kernelHeap, addr);
}