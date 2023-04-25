#include "heap.h"
#include "kernel.h"
#include <stdbool.h>
#include "status.h" 
#include "mem/mem.h"

static int heap_validate_table(void* ptr, void* end, heap_table* table)
{
    int res = 0;

    size_t table_size = (size_t)(end - ptr);
    size_t total_block = table_size / HEAP_BLOCK_SIZE;
    if (table->total != total_block)
    {
        res = -EINVARG;
        goto out;
    }
out:
    return res;
}

static bool heap_validate_alignement(void* ptr)
{
    return ((unsigned int)ptr % HEAP_BLOCK_SIZE) == 0;
}

int heap_create(heap* heap_obj, void* ptr, void* end, heap_table* table)
{
    int result = 0;
    
    if (!heap_validate_alignement(ptr) || !heap_validate_alignement(end))
    {
        result = -EINVARG;
        goto out;
    }

    memset(heap_obj, 0, sizeof(heap));
    heap_obj->start_addr = ptr;
    heap_obj->table = table;

    result = heap_validate_table(ptr, end, table);
    if (result < 0)
    {
        goto out;
    }

    size_t table_size = sizeof(HEAP_BLOCK_TABLE_ENTRY) * table->total;
    memset(table->entries, HEAP_BLOCK_FREE, table_size);

out:
    return result;
}

static int heap_get_entry_type(HEAP_BLOCK_TABLE_ENTRY entry)
{
    return entry & 0x0f;
}

int heap_get_start_block(heap* heap, uint32_t total_blocks)
{
    heap_table* table = heap->table;
    uint32_t current_block = 0;
    int start_free_block = -1;
    for (size_t i = 0; i < table->total; i++)
    {
        if (heap_get_entry_type(table->entries[i]) != HEAP_BLOCK_FREE)
        {
            current_block = 0;
            start_free_block = -1;
            continue;  
        }

        // current block is free
        if (start_free_block == -1)
        {
            start_free_block = i;
        }
        current_block++;
        if (current_block == total_blocks)
        {
            break;
        }

    }

    if (start_free_block == -1)
    {
        return -ENOMEM;
    }

    return start_free_block;

}

void* heap_block_to_address(heap* heap, int block)
{
    return heap->start_addr + (block * HEAP_BLOCK_SIZE);
}

void heap_mark_block_taken(heap* heap, int start_block, int total_block)
{
    int end_block = (start_block + total_block) -1;

    HEAP_BLOCK_TABLE_ENTRY entry = HEAP_BLOCK_TAKEN | HEAP_BLOCK_IS_FIRST;
    if (total_block > 1)
    {
        entry |= HEAP_BLOCK_HAS_NEXT;
    }

    for (int i = start_block; i <= end_block; i++)
    {
        heap->table->entries[i] = entry;
        entry = HEAP_BLOCK_TAKEN;
        if (i!= end_block - 1)
        {
            entry |= HEAP_BLOCK_HAS_NEXT;
        }
    }
    
}

void* heap_malloc_blocks(heap* heap, uint32_t total_blocks) 
{
    void* address = 0;
    int start_block = heap_get_start_block(heap, total_blocks);

    if (start_block < 0)
    {
        goto out;
    }
    address = heap_block_to_address(heap, start_block);

    heap_mark_block_taken(heap, start_block, total_blocks);

out:
    return address;
}

static uint32_t heap_align_value_to_upper(uint32_t value)
{
    uint32_t result;
    
    if ((value % HEAP_BLOCK_SIZE) == 0)
    {
       result = value;
    } else {
        result = (value - (value % HEAP_BLOCK_SIZE));
        result += HEAP_BLOCK_SIZE;

    }

    return result;
}

void* heap_malloc(heap* heap, size_t size)
{
    size_t aligned_size = heap_align_value_to_upper(size);
    uint32_t total_block = aligned_size / HEAP_BLOCK_SIZE;

    return heap_malloc_blocks(heap, total_block);
}

int heap_address_to_block(heap* heap,void* address)
{
    return ((int) (address - heap->start_addr)) / HEAP_BLOCK_SIZE;
}

void  heap_mark_block_free(heap* heap, int starting_block)
{
    heap_table* heap_table = heap->table;
    for (int i = starting_block; i < heap_table->total; i++)
    {
        HEAP_BLOCK_TABLE_ENTRY entry = heap_table->entries[i];
        heap_table->entries[i] = HEAP_BLOCK_FREE;
        if(!(entry & HEAP_BLOCK_HAS_NEXT))
        {
            break;
        }
    }
    
}

void heap_free(heap* heap, void* ptr)
{
    heap_mark_block_free(heap, heap_address_to_block(heap, ptr));
}