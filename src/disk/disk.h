#ifndef DISK_H
#define DISK_H

#include "stdint.h"
#include "fs/file.h"


typedef unsigned int PEACHOS_DISK_TYPE;

#define PEACHOS_DISK_TYPE_REAL 0

struct disk
{
    PEACHOS_DISK_TYPE diskType;
    int sectorSize;

    // The id of the disk
    int id;

    struct fileSystem* filesystem;

    // private data of the disk
    void* fs_private;
};

void disk_search_and_init();
struct disk* disk_get(int index);
int disk_read_block(struct disk* idisk, int lba, int total, void* buff);

#endif