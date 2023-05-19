//
// Created by Yoann on 18/05/2023.
//

#ifndef UDEMY_FILE_H
#define UDEMY_FILE_H
#include "pparser.h"



typedef unsigned int FILE_SEEK_MODE;

enum
{
    SEEK_SET,
    SEEK_CURRENT,
    SEEK_END
};
typedef unsigned int FILE_MODE;
enum
{
    FILE_READ,
    FILE_WRITE,
    FILE_APPEND,
    FILE_INVALID
};

struct disk;
typedef void*(*FS_OPEN_FUNCTION)(struct disk* disk, struct path_part* path, FILE_MODE mode);
typedef int(*FS_RESOLVE_FUNCTION)(struct disk* disk);

typedef struct {
    FS_RESOLVE_FUNCTION resolve;
    FS_OPEN_FUNCTION open;

    char name[20];
} filesystem;

typedef struct {
    int index;
    filesystem* fs;

    void* private;
    struct disk* disk;
} file_descriptor;



void fs_init();
int fopen(const char* filename, const char* mode);
void fs_insert_filesystem(filesystem* fs);
filesystem * fs_resolve(struct disk* disk);


#endif //UDEMY_FILE_H
