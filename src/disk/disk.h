#ifndef DISK_H
#define DISK_H

typedef unsigned int DISK_TYPE;

#define DISK_TYPE_REAL 0

typedef struct 
{
    DISK_TYPE type;
    int sector_size;

} disk;


void disk_search_and_init();
disk *disk_get(int idx);
int disk_read_block(disk *idisk, unsigned int lba, int total, void *buff);

#endif