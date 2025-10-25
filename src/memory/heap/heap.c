#include "memory/heap/heap.h"
#include "config.h"
#include "status.h"

static void InitializeHeapTable(heapTable* heapTable)
{
    for(size_t i=0; i<heapTable->total; i++)
    {
        heapTable->entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;
    }

    return;
}

int CreateHeap(heap* heap, uint32_t heapTableStAddr, void* startAddr, void* endAddr)
{
    int status = 0;

    // return if total size of the heap needed is not aligned with HEAP_BLOCK_SIZE_BYTES
    // as heap will be allocated in units of HEAP_BLOCK_SIZE_BYTES
    if ((((uint32_t)endAddr-(uint32_t)startAddr) + 1) % HEAP_BLOCK_SIZE_BYTES != 0)
    {
        status = -EINVARG;
        // using goto instead of return here to handle any error handling for the error
        // not sure why not handle this error by calling error handler and have return statement after 
        goto out;
    }

    // Initialize heap start address
    heap->saddr = startAddr;

    // Initialize heap table total number of entries
    heap->table.total = ((((uint32_t)endAddr-(uint32_t)startAddr) + 1) / HEAP_BLOCK_SIZE_BYTES);

    // heap table start address
    heap->table.entries = (HEAP_BLOCK_TABLE_ENTRY*)heapTableStAddr;

    // Initialize all the heap table entries as free
    InitializeHeapTable(&heap->table);

    out:
        return status;
}

size_t BlocksToAllocate(size_t bytes)
{

    // bytes is multiple of HEAP_BLOCK_SIZE_BYTES
    if(bytes%HEAP_BLOCK_SIZE_BYTES == 0)
    {
        return bytes/HEAP_BLOCK_SIZE_BYTES;
    }

    // Allocate more bytes than required by rounding up the bytes required to be multiple of HEAP_BLOCK_SIZE_BYTES
    return (bytes + (HEAP_BLOCK_SIZE_BYTES - (bytes%HEAP_BLOCK_SIZE_BYTES))) / HEAP_BLOCK_SIZE_BYTES;
}

static int searchForContiguousBlocks(heap* heap, size_t blocksToAllocate)
{
    int bs = -1, bc = 0;

    for(int i=0; i<heap->table.total; i++)
    {
        if((heap->table.entries[i] | HEAP_BLOCK_TABLE_ENTRY_FREE) == 0)
        {
            if(bc == 0)
            {
                bs = i;
            }

            bc++;

            if(bc == blocksToAllocate)
            {
                return bs;
            }
        }
        else
        {
            bs = -1;
            bc = 0;
        }
    }

    return bs;
}

static void UpdateHeapTableOfAllocation(heap* heap, int stIndex, size_t blocksToAllocate)
{
    for(int i=stIndex; i < stIndex + blocksToAllocate-1; i++)
    {
        heap->table.entries[i] = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_HAS_NEXT;
    }

    // Initialize last entry
    heap->table.entries[stIndex + blocksToAllocate - 1] = HEAP_BLOCK_TABLE_ENTRY_TAKEN | HEAP_BLOCK_IS_LAST;
}

void* HeapMalloc(heap* heap, size_t bytes)
{
    size_t blocksToAllocate = BlocksToAllocate(bytes);

    int si = searchForContiguousBlocks(heap, blocksToAllocate);

    if(si == -1)
    {
        return NULL;
    }

    UpdateHeapTableOfAllocation(heap, si, blocksToAllocate);

    return (void*)((uint32_t)heap->saddr + (si*HEAP_BLOCK_SIZE_BYTES));
}

int HeapFree(heap* heap, void* addr)
{
    if((uint32_t)addr % HEAP_BLOCK_SIZE_BYTES != 0)
    {
        return -EINVARG;
    }

    int si = ((uint32_t)addr - (uint32_t)heap->saddr)/HEAP_BLOCK_SIZE_BYTES;

    int last = si;

    for(int i=si; i < heap->table.total; i++)
    {
        if((heap->table.entries[i] & HEAP_BLOCK_IS_LAST ) == HEAP_BLOCK_IS_LAST)
        {
            last = i;
            i = heap->table.total;
        }

        heap->table.entries[i] = HEAP_BLOCK_TABLE_ENTRY_FREE;  
    }

    heap->table.entries[last] = HEAP_BLOCK_TABLE_ENTRY_FREE;  

    return 0;
}


