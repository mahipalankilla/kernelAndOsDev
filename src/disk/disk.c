#include "io/io.h"
#include "disk.h"
#include "memory/memory.h"
#include "config.h"
#include "status.h"
#include "fs/file.h"

struct disk disk;
int disk_read_sector(int startLba, int totalSectors, void* buff)
{

    // Refer https://wiki.osdev.org/ATA_read/write_sectors for ATA read
    outb(0x1F6, ((startLba >> 24) | 0xE0));

    // total number of sectors that can be read by using this API is limited by 256
    outb(0x01F2, totalSectors);

    outb(0x1F3, (unsigned char)(startLba & 0xff));

    outb(0x1F4, (unsigned char)(startLba >> 8));

    outb(0x1F5, (unsigned char)(startLba >> 16));

    // read command with retry
    outb(0x1F7, 0x20);

    unsigned short *ptr = (unsigned short*)buff;

    for (int sectorsRead = 0; sectorsRead < totalSectors ; sectorsRead++)
    {
        // poll until sector is ready to read
        while (!(insb(0x1F7) & 0x8));

        // read full sector each word at a time (word here as this is waht is available with insw)
        for (int wordsRead = 0; wordsRead < 256; wordsRead++)
        {
            *ptr = insw(0x1F0);
            ptr++;
        }
    }

    return 0;
}

void disk_search_and_init()
{
    memset(&disk, 0, sizeof(disk));
    disk.diskType = PEACHOS_DISK_TYPE_REAL;
    disk.sectorSize = PEACHOS_SECTOR_SIZE;
    disk.id = 0;
    disk.filesystem = fs_resolve(&disk);
}

struct disk* disk_get(int index)
{
    if (index != 0)
    {
        return 0;
    }

    return &disk;
}

int disk_read_block(struct disk* idisk, int lba, int total, void* buff)
{
    if (idisk != &disk)
    {
        return -EIO;
    }

    return disk_read_sector(lba, total, buff);
}