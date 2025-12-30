#ifndef PATHPARSER_H
#define PATHPARSER_H
#include "string/string.h"

struct path_root
{
    int driveNo;
    struct path_part* first;
};

struct path_part
{
    const char* part;
    struct path_part* next;
};

void pathparser_free(struct path_root* root);
struct path_root* pathparser_parse(const char* path, const char* curr_directory_path);

#endif