#include "kheap.h"
#include "heap.h"
#include "config.h"
#include "kernel.h"
#include "mem/mem.h"

heap kernel_heap;
heap_table kernel_heap_table;


void kheap_init()
{
    int total_table_entries = HEAP_SIZE_BYTES / HEAP_BLOCK_SIZE;

    kernel_heap_table.entries = (HEAP_BLOCK_TABLE_ENTRY*)(HEAP_TABLE_ADDRESS);
    kernel_heap_table.total = total_table_entries;

    void* end_ptr = (void*) (HEAP_ADDRESS + HEAP_SIZE_BYTES);
    int res = heap_create(&kernel_heap, (void*) (HEAP_ADDRESS), end_ptr, &kernel_heap_table);

    if (res < 0)
    {
        print("heap kernel failed");
    }
}

void* kmalloc(size_t size)
{
    return heap_malloc(&kernel_heap, size);
}

void* kzalloc(size_t size)
{
    void* ptr = kmalloc(size);
    memset(ptr, 0x00, size);
    return ptr;
}

void kfree(void* ptr)
{
    heap_free(&kernel_heap, ptr);
}