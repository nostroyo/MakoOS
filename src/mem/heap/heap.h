#ifndef HEAP_H
#define HEAP_H
#include "config.h"
#include <stdint.h>
#include <stddef.h>

#define HEAP_BLOCK_TAKEN 0x01
#define HEAP_BLOCK_FREE 0x00
#define HEAP_BLOCK_HAS_NEXT 0b10000000
#define HEAP_BLOCK_IS_FIRST  0b01000000 

typedef unsigned char HEAP_BLOCK_TABLE_ENTRY;

typedef struct 
{
    HEAP_BLOCK_TABLE_ENTRY* entries;
    size_t total;

} heap_table ;

typedef struct 
{
    heap_table* table;
    void* start_addr;
} heap;

int heap_create(heap *heap, void *ptr, void *end, heap_table *table);
void *heap_malloc(heap *heap, size_t size);

void heap_free(heap *heap, void *ptr);

#endif