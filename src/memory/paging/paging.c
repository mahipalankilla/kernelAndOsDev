#include <memory/paging/paging.h>
#include <memory/heap/kheap.h>
#include "status.h"

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

void paging_switch(struct paging_4gb_chunk* chunk)
{
    paging_load_directory(chunk->directory_entry);
    current_directory = chunk->directory_entry;
}

void modifyPageTableEntry(void* virtualAddress, uint32_t phyAddr)
{
    uint32_t pageDirectoryIndex = ((uint32_t)virtualAddress / (PAGE_TABLE_ENTRIES_PER_TABLE * PAGE_SIZE_IN_BYTES));
    uint32_t* pageTableStartAddr = (uint32_t*)(current_directory[pageDirectoryIndex] & 0xfffff000);

    uint32_t pageTableIndex = ((uint32_t)virtualAddress % (PAGE_TABLE_ENTRIES_PER_TABLE * PAGE_SIZE_IN_BYTES)) / PAGE_SIZE_IN_BYTES;
    
    pageTableStartAddr[pageTableIndex] = ((pageTableStartAddr[pageTableIndex] & 0x00000FFF) | phyAddr);
}

void paging_free_4gb(struct paging_4gb_chunk* chunk)
{
    for (int i = 0; i < PAGE_TABLE_ENTRIES_PER_TABLE; i++)
    {
        uint32_t entry = chunk->directory_entry[i];
        uint32_t* table = (uint32_t*)(entry & 0xfffff000);
        kFree(table);
    }

    kFree(chunk->directory_entry);
    kFree(chunk);
}


int paging_set(uint32_t* directory, void* virt, void* phy)
{
    if (((unsigned int) virt % PAGE_SIZE_IN_BYTES) || (((unsigned int)phy & 0xfffff000) % PAGE_SIZE_IN_BYTES))
    {
        return -EINVARG;
    }

    uint32_t pageDirectoryIndex = ((uint32_t)virt / (PAGE_TABLE_ENTRIES_PER_TABLE * PAGE_SIZE_IN_BYTES));
    uint32_t* pageTableStartAddr = (uint32_t*)(directory[pageDirectoryIndex] & 0xfffff000);

    uint32_t pageTableIndex = ((uint32_t)virt % (PAGE_TABLE_ENTRIES_PER_TABLE * PAGE_SIZE_IN_BYTES)) / PAGE_SIZE_IN_BYTES;
    
    pageTableStartAddr[pageTableIndex] = (uint32_t)phy;  

    return PEACHOS_ALL_OK;
}

int paging_map(struct paging_4gb_chunk* chunk, void* virt, void* phy, int flags)
{
    if (((unsigned int) virt % PAGE_SIZE_IN_BYTES) || ((unsigned int)phy % PAGE_SIZE_IN_BYTES))
    {
        return -EINVARG;
    }

    return  paging_set(chunk->directory_entry, virt, (void*)(((uint32_t) phy) | flags));

}

int paging_map_range(struct paging_4gb_chunk* directory, void* virt, void* phy, int count, int flags)
{
    int res = 0;
    for (int i=0; i< count; i++)
    {
        res = paging_map(directory, virt, phy, flags);
        if (res < 0)
            break;
        virt += PAGE_SIZE_IN_BYTES;
        phy += PAGE_SIZE_IN_BYTES;
    }

    return res;
}

int paging_map_to(struct paging_4gb_chunk* directory, void* virt, void* phy, void* phy_end, int flags)
{
    int res = 0;

    if ((uint32_t)virt % PAGE_SIZE_IN_BYTES)
    {
        res = -EINVARG;
        goto out;
    }

    if ((uint32_t)phy % PAGE_SIZE_IN_BYTES)
    {
        res = -EINVARG;
        goto out;
    }

    if ((uint32_t)phy_end % PAGE_SIZE_IN_BYTES)
    {
        res = -EINVARG;
        goto out;
    } 

    uint32_t total_bytes = phy_end - phy;
    uint32_t total_pages = total_bytes / PAGE_SIZE_IN_BYTES;
    res = paging_map_range(directory, virt, phy, total_pages, flags);
    if (res < 0)
    {
        res = -EIO;
        goto out;
    }

out:
    return res;
}

void* paging_align_addres(void* ptr)
{
    uint32_t address = (uint32_t)ptr;

    if (address % PAGE_SIZE_IN_BYTES)
    {
        return (void*)(address + (PAGE_SIZE_IN_BYTES - (address % PAGE_SIZE_IN_BYTES)));
    }
    
    return ptr;
}

void paging_get_indexes(void* virt, uint32_t* directory_index_ptr, uint32_t* table_index_ptr)
{
    *directory_index_ptr = (uint32_t)(virt) / (PAGE_SIZE_IN_BYTES * PAGE_TABLE_ENTRIES_PER_TABLE);

    *table_index_ptr = (uint32_t)(virt) % PAGE_SIZE_IN_BYTES;

    return;
}
uint32_t paging_get(uint32_t* directory, void* virt)
{
    uint32_t directory_index = 0;
    uint32_t table_index = 0;
    paging_get_indexes(virt, &directory_index, &table_index);
    uint32_t directoryEntry = directory[directory_index];
    uint32_t* pageTable = (uint32_t*)(directoryEntry & 0xfffff000);
    return pageTable[table_index];
}