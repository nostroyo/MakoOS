#include "IO/io.h"
#include "disk.h"
#include "mem/mem.h"
#include "config.h"
#include "status.h"

disk disk_data;

int disk_read_sector(int lba, int total, void* buf)
{
    outb(0x1F6, (lba >> 24) | 0xE0);
    outb(0x1F2, total);
    outb(0x1F3, (unsigned char)(lba & 0xff));
    outb(0x1F4, (unsigned char)(lba >> 8));
    outb(0x1F5, (unsigned char)(lba >> 16));
    outb(0x1F7, 0x20);

    unsigned short* ptr = (unsigned short*) buf;
    for (int b = 0; b < total; b++)
    {
        // Wait for the buffer to be ready
        char c = insb(0x1F7);
        while(!(c & 0x08))
        {
            c = insb(0x1F7);
        }

        // Copy from hard disk to memory
        for (int i = 0; i < 256; i++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }

    }
    return 0;
}

void disk_search_and_init()
{
    memset(&disk_data, 0, sizeof(disk));
    disk_data.type = DISK_TYPE_REAL;
    disk_data.sector_size = SECTOR_SIZE;

}

disk* disk_get(int idx)
{
    if (idx != 0)
    {return 0;}

    return &disk_data;
}

int disk_read_block(disk* idisk, unsigned int lba, int total, void* buff)
{
    if (idisk != &disk_data)
    {
        return -STATUS_EIO;
    }

    return disk_read_sector(lba, total, buff);
    
}
