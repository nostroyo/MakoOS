
#ifndef PAGING_H
#define PAGING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define PAGING_CACHE_DISABLED   0b00010000
#define PAGINGWRITE_THROUGH     0b00001000
#define PAGING_ACCESS_FROM_ALL  0b00000100
#define PAGING_IS_WRITABLE      0b00000010
#define PAGING_IS_PRESENT       0b00000001


#define PAGING_TOTAL_ENTRIES_TABLE 1024
#define PAGIN_PAGE_SIZE 4096

typedef struct  {
    uint32_t* directory_entry;

} paging_four_GB_chunck;

paging_four_GB_chunck *paging_new_4gb(uint8_t flags);

void paging_switch(uint32_t *directory);
uint32_t *paging_get_directory(paging_four_GB_chunck *chunk);
bool paging_is_aligned(void *addr);
int paging_set(uint32_t *directory, void *virtual_addr, uint32_t value);
void enable_paging();

#endif
