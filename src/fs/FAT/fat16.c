//
// Created by Yoann on 19/05/2023.
//

#include "fat16.h"
#include "status.h"
#include "string/string.h"
#include "../../mem/heap/kheap.h"
#include "../../disk/streamer.h"
#include "../../mem/mem.h"
#include <stdint.h>

#define FAT16_SIG 0x29
#define FAT16_FAT_ENTRY_SIZE 0x02
#define FAT16_BAD_SECTOR 0xFF7
#define FAT16_UNUSED 0x00

typedef unsigned int FAT_ITEM_TYPE;
#define FAT_TYPE_DIR 0
#define FAT_TYPE_FILE 1

#define FAT_FILE_RO 0x01
#define FAT_FILE_HIDDEN 0x02
#define FAT_FILE_SYSTEM 0x04
#define FAT_FILE_VOL_LABEL 0x08
#define FAT_FILE_SUB_DIR 0x10
#define FAT_FILE_ARCHIVED 0x20
#define FAT_FILE_DEVICE 0x40
#define FAT_FILE_RESERVED 0x80

typedef struct __attribute__((packed))
{
    uint8_t drive_number;
    uint8_t win_nt_bit;
    uint8_t sig;
    uint32_t vol_id;
    uint8_t vol_id_str[11];
    uint8_t sys_id_str[8];

} fat_header_extended;

typedef struct __attribute__((packed))
{
    uint8_t short_jmp_ins[3];
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_dir_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t sectors_big;
} fat_header;

typedef struct
{
    fat_header primary_header;
    union fat_h_e
    {
        fat_header_extended extended;
    }shared;
} fat_h;

typedef struct __attribute__((packed))
{
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t creation_time_tenth_of_sec;
    uint16_t creation_time;
    uint16_t create_date;
    uint16_t last_access;
    uint16_t hi_16b_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_day;
    uint16_t lo_16b_first_cluster;
    uint32_t filesize;
} fat_dir_item;

typedef struct
{
    fat_dir_item* item;
    int total;
    int sector_pos;
    int ending_sector_pos;

} fat_dir;

typedef struct
{
    union
    {
        fat_dir_item* item;
        fat_dir* dir;
    };

    FAT_ITEM_TYPE type;
} fat_item;

typedef struct
{
    fat_item * item;
    uint32_t pos;
} fat_item_descriptor;

typedef struct
{
    fat_h header;
    fat_dir root_dir;
    struct disk_stream* cluster_read_stream;
    struct disk_stream* fat_read_stream;

    struct disk_stream* directory_stream;



} fat_private;

int fat16_resolve(struct disk* disk);
void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode);

filesystem fat16_fs = {
    .resolve = fat16_resolve,
    .open = fat16_open,

};

int fat16_sector_to_absolute(struct disk* disk, int sector)
{
    return sector * disk->sector_size;
}

int fat_get_total_items_for_dir(struct disk* disk, uint32_t start_sector)
{
    fat_dir_item item;
    fat_dir_item empty;

    memset(&empty, 0x00, sizeof(empty));
    fat_private * fat_internal = disk->fs_private;

    int res = 0;
    int i = 0;
    int directory_start_pos = start_sector * disk->sector_size;
    struct disk_stream* stream = fat_internal->directory_stream;
    if (diskstreamer_seek(stream, directory_start_pos) != STATUS_OK)
    {
        res = -STATUS_EIO;
        goto out;
    }
    while (1)
    {
        if (diskstreamer_read(stream, &item, sizeof(item)) != STATUS_OK)
        {
            res = -STATUS_EIO;
            goto out;
        }

        if (item.filename[0] == 0x00)
        {
            break;
        }

        if (item.filename[0] == 0xE5)
        {
            continue;
        }

        i++;
    }
    res = i;
out:
    return res;
}

int fat16_get_root_directory(struct disk* disk, fat_private* fat_internal, fat_dir* directory)
{
    int res = 0;

    fat_header* primary_header = &fat_internal->header.primary_header;
    int root_dir_sector_position = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
    int root_dir_entries = fat_internal->header.primary_header.root_dir_entries;
    int root_dir_size = (root_dir_entries * sizeof(fat_dir_item));
    int total_sectors = root_dir_size / disk->sector_size;
    if (root_dir_size % disk->sector_size != 0)
    {
        total_sectors++;
    }


    int total_item = fat_get_total_items_for_dir(disk, root_dir_sector_position);
    fat_dir_item * dir = kzalloc(root_dir_size);
    if (!dir)
    {
        res = -STATUS_EIO;
        goto out;
    }

    struct disk_stream* stream = fat_internal->directory_stream;
    if (diskstreamer_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_position)) != STATUS_OK)
    {
        res = -STATUS_EIO;
        goto out;
    }

    directory->item = dir;
    directory->total = total_item;
    directory->sector_pos = root_dir_sector_position;
    directory->ending_sector_pos = root_dir_sector_position + (root_dir_size / disk->sector_size);

out:
    return res;
}

filesystem* fat16_init()
{
    strcpy(fat16_fs.name, "FAT16");
    return &fat16_fs;

}

void* fat16_open(struct disk* disk, struct path_part* path, FILE_MODE mode)
{

    return 0;
}

static void fat16_init_private(struct disk* disk, fat_private* internal)
{
    memset(internal, 0x00, sizeof(fat_private));
    internal->cluster_read_stream = diskstreamer_new(disk->id);
    internal->fat_read_stream = diskstreamer_new(disk->id);
    internal->directory_stream = diskstreamer_new(disk->id);
}

int fat16_resolve(struct disk* disk)
{
    int res = 0;
    fat_private *fat_private = kzalloc(sizeof(fat_private));
    fat16_init_private(disk, fat_private);

    disk->fs_private = fat_private;
    disk->fs = &fat16_fs;

    struct disk_stream *stream = diskstreamer_new(disk->id);
    if (!stream)
    {
        res = -ENOMEM;
        goto out;
    }

    if (diskstreamer_read(stream, &fat_private->header, sizeof(fat_private->header)) != STATUS_OK)
    {
        res = -STATUS_EIO;
        goto out;
    }

    if (fat_private->header.shared.extended.sig != 0x29)
    {
        res = -EFSNOTUS;
        goto out;
    }

    if (fat16_get_root_directory(disk, fat_private, &fat_private->root_dir) != STATUS_OK)
    {
        res = -STATUS_EIO;
        goto out;
    }
    out:
    if (stream)
    {
        diskstreamer_close(stream);
    }

    if (res < 0)
    {
        kfree(fat_private);
        disk->fs_private = 0;
    }
    return res;
}