#include "paging.h"
#include "mem/heap/kheap.h"
#include "status.h"


static uint32_t* current_directory = 0;

void paging_load_directory(uint32_t* directory);

paging_four_GB_chunck* paging_new_4gb(uint8_t flags)
{
    uint32_t* directory = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_TABLE);
    int offset = 0;
    for (size_t i = 0; i < PAGING_TOTAL_ENTRIES_TABLE; i++)
    {
        uint32_t* entry = kzalloc(sizeof(uint32_t) * PAGING_TOTAL_ENTRIES_TABLE);
        for (size_t j = 0; j < PAGING_TOTAL_ENTRIES_TABLE; j++)
        {
            entry[j] = (offset + (j * PAGIN_PAGE_SIZE)) | flags; 
        }
        offset += (PAGING_TOTAL_ENTRIES_TABLE * PAGIN_PAGE_SIZE);
        directory[i] = (uint32_t)entry | flags | PAGING_IS_WRITABLE;
    }
    
    paging_four_GB_chunck* chunk = kzalloc(sizeof(paging_four_GB_chunck));
    chunk->directory_entry = directory;
    return chunk;
}

void paging_switch(uint32_t* directory)
{
    paging_load_directory(directory);
    current_directory = directory;
}

uint32_t* paging_get_directory(paging_four_GB_chunck* chunk)
{
    return chunk->directory_entry;
}

bool paging_is_aligned(void* addr)
{
    return ((uint32_t)addr % PAGIN_PAGE_SIZE) == 0;
}

int paging_get_indexes(void* virtual_addr, uint32_t* directory_idx_out, uint32_t* table_idx)
{
    int res = 0;
    if (!paging_is_aligned(virtual_addr))
    {
        res = -EINVARG;
        goto out;
    }

    *directory_idx_out = ((uint32_t)virtual_addr / (PAGING_TOTAL_ENTRIES_TABLE * PAGIN_PAGE_SIZE));
    *table_idx = ((uint32_t) virtual_addr % (PAGING_TOTAL_ENTRIES_TABLE * PAGIN_PAGE_SIZE) / PAGIN_PAGE_SIZE); 

out:
    return res;
}

int paging_set(uint32_t* directory, void* virtual_addr, uint32_t value)
{
    if (!paging_is_aligned(virtual_addr))
    {
        return -EINVARG;
    }

    uint32_t directory_idx = 0;
    uint32_t table_idx = 0;

    int res = paging_get_indexes(virtual_addr, &directory_idx, &table_idx);
    if (res < 0)
    {
        return res;
    }

    uint32_t entry = directory[directory_idx];
    uint32_t* table = (uint32_t*)(entry & 0xfffff000);

    table[table_idx] = value;

    return 0;
}