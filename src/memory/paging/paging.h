#ifndef PAGING
#define PAGING

#include <stddef.h>
#include <config.h>
#include <stdint.h>

#define PAGING_IS_PRESENT       0b000000001
#define PAGING_IS_WRITEABLE     0b000000010
#define PAGING_CACHE_DISABLE    0b000010000
#define PAGING_WRITE_THROUGH    0b000001000
#define PAGING_ACCESS_FROM_ALL  0b000000100

#define PAGE_SIZE_IN_BYTES HEAP_BLOCK_SIZE_BYTES

typedef unsigned int pageEntry;

typedef char static_assert_uint_size[(sizeof(unsigned int) == 4) ? 1 : -1];

#define PAGE_TABLE_ENTRIES_PER_TABLE HEAP_BLOCK_SIZE_BYTES/sizeof(unsigned int)

typedef pageEntry pageTable[PAGE_TABLE_ENTRIES_PER_TABLE];

typedef pageEntry pageDirectory[PAGE_TABLE_ENTRIES_PER_TABLE];

struct paging_4gb_chunk
{
    uint32_t* directory_entry;
};

void paging_switch(uint32_t* directory);
struct paging_4gb_chunk* paging_new_4gb(uint8_t flags);
uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk);
void enable_paging();
void modifyPageTableEntry(void* virtualAddress, uint32_t phyAddr);

#endif // PAGING