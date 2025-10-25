#include "memory/heap/kheap.h"
#include "memory/heap/heap.h"
#include "memory/memory.h"

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

void* kZalloc(size_t bytes)
{
    void* ptr = kMalloc(bytes);

    if (!ptr)
        return 0;

    memset(ptr, 0x00, bytes);

    return ptr;
}