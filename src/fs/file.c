//
// Created by Yoann on 18/05/2023.
//

#include "file.h"
#include "../config.h"
#include "mem/mem.h"
#include "mem/heap/kheap.h"
#include "status.h"
#include "kernel.h"
#include "FAT/fat16.h"

filesystem* filesystems[MAX_FS];
file_descriptor * fileDescriptors[MAX_FILE_DESCR];

static filesystem** fs_get_free_fs()
{
    int i = 0;
    for (i = 0; i < MAX_FS; ++i) {
        if(filesystems[i] == 0)
        {
            return &filesystems[i];
        }
    }

    return 0;
}

void fs_insert_filesystem(filesystem* file_system)
{
    filesystem** fs;

    fs = fs_get_free_fs();
    if (!fs)
    {
        print("FS error");
        while(1) {}
    }

    *fs = file_system;
}

static void fs_static_load()
{
    fs_insert_filesystem(fat16_init());
}

void fs_load()
{
    memset(filesystems, 0x00, sizeof(filesystems));
    fs_static_load();
}

void fs_init()
{
    memset(fileDescriptors, 0x00, sizeof(fileDescriptors));
    fs_load();
}

static int file_new_descriptor(file_descriptor ** desc_out)
{
    int res = -ENOMEM;
    for (int i = 0; i < MAX_FILE_DESCR; ++i) {
        if (fileDescriptors[i] == 0)
        {
            file_descriptor* desc = kzalloc(sizeof(file_descriptor));
            // start at 1
            desc->index = i + 1;
            fileDescriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static file_descriptor * file_get_descriptor(int fd)
{
    if (fd <= 0 || fd >= MAX_FILE_DESCR)
    {
        return 0;
    }

    int index = fd - 1;
    return fileDescriptors[index];
}

filesystem * fs_resolve(struct disk* disk)
{
    filesystem* fs = 0;
    for (int i = 0; i < MAX_FS; ++i) {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
        {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

int fopen(const char* filename, const char* mode)
{
    return -STATUS_EIO;
}