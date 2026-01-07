#include "streamer.h"
#include "memory/heap/kheap.h"
#include "config.h"
#include "disk.h"

struct disk_stream* diskSteamerNew(int diskNo)
{
    struct disk* disk = disk_get(diskNo);
    if (!disk)
    {
        return 0;
    }

    struct disk_stream* streamer = kZalloc(sizeof(struct disk_stream));
    streamer->pos = 0;
    streamer->disk = disk;

    return streamer;
}

int diskSteamerSeek(struct disk_stream* stream, int pos)
{
    stream->pos = pos;
    return 0;
}

int diskSteamerRead(struct disk_stream* stream, void* out, int total)
{
    int sector = stream->pos / PEACHOS_SECTOR_SIZE;
    int offset = stream->pos % PEACHOS_SECTOR_SIZE;

    char buff[PEACHOS_SECTOR_SIZE];

    int total_to_read = total;

    bool overflow = (offset + total_to_read) >= PEACHOS_SECTOR_SIZE;

    int res = disk_read_block(stream->disk, sector, 1, buff);
    if (res < 0)
    {
        goto out;
    }

    if (overflow)
    {
        total_to_read = PEACHOS_SECTOR_SIZE - offset;
    }

    for(int i = 0; i < total_to_read; i++)
    {
        *(char*)out++ = buff[i+offset];
    }

    // Adjust the stream
    stream->pos += total_to_read;

    if (overflow)
    {
        res = diskSteamerRead(stream, out, total - total_to_read);
    }

    out:
        return res;
}

void diskStreamerClose(struct disk_stream* stream)
{
    kFree(stream);
}
