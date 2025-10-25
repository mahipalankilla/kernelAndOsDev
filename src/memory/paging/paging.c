#include <memory/paging/paging.h>
#include <memory/heap/kheap.h>

// This is to make sure that the size of page table or directory entry is 4 bytes
static_assert_uint_size test_array;

void paging_load_directory(uint32_t* directory);
 
static uint32_t* current_directory = 0;

struct paging_4gb_chunk* paging_new_4gb(uint8_t flags)
{
    uint32_t* directory = (uint32_t*)kZalloc(sizeof(uint32_t) * PAGE_TABLE_ENTRIES_PER_TABLE);

    for (int pageDirectoryIndex = 0; pageDirectoryIndex < PAGE_TABLE_ENTRIES_PER_TABLE; pageDirectoryIndex++)
    {
        uint32_t* entry = (uint32_t*)kZalloc(sizeof(uint32_t) * PAGE_TABLE_ENTRIES_PER_TABLE);

        uint32_t pageTableStartAddr = pageDirectoryIndex * PAGE_TABLE_ENTRIES_PER_TABLE * PAGE_SIZE_IN_BYTES;

        for (int pageTableIndex = 0; pageTableIndex < PAGE_TABLE_ENTRIES_PER_TABLE; pageTableIndex++)
        {
            entry[pageTableIndex] = (pageTableStartAddr + (pageTableIndex * PAGE_SIZE_IN_BYTES)) | flags; 
        }

        directory[pageDirectoryIndex] = (uint32_t)entry | flags | PAGING_IS_WRITEABLE;
    }

    struct paging_4gb_chunk* chunk_4gb = (struct paging_4gb_chunk*)kZalloc(sizeof(struct paging_4gb_chunk));

    chunk_4gb->directory_entry = directory;

    return chunk_4gb;

}

uint32_t* paging_4gb_chunk_get_directory(struct paging_4gb_chunk* chunk)
{
    return chunk->directory_entry;
}

void paging_switch(uint32_t* directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

void modifyPageTableEntry(void* virtualAddress, uint32_t phyAddr)
{
    uint32_t pageDirectoryIndex = ((uint32_t)virtualAddress / PAGE_TABLE_ENTRIES_PER_TABLE * PAGE_SIZE_IN_BYTES);
    uint32_t* pageTableStartAddr = (uint32_t*)(current_directory[pageDirectoryIndex] & 0xfffff000);

    uint32_t pageTableIndex = ((uint32_t)virtualAddress % (PAGE_TABLE_ENTRIES_PER_TABLE * PAGE_SIZE_IN_BYTES)) / PAGE_SIZE_IN_BYTES;
    
    pageTableStartAddr[pageTableIndex] = ((pageTableStartAddr[pageTableIndex] & 0x00000FFF) | phyAddr);
}