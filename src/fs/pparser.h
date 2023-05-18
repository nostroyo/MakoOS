//
// Created by Yoann on 10/05/2023.
//

#ifndef UDEMY_PPARSER_H
#define UDEMY_PPARSER_H
struct path_part
{
    const char* part;
    struct path_part* next;
};

typedef struct
{
    int drive_no;
    struct path_part* first;
} path_root;

path_root* pathparser_parse(const char* path, const char* current_directory_path);
void pathparser_free(path_root* root);

#endif //UDEMY_PPARSER_H
