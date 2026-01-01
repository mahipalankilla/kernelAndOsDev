#ifndef STREAMER_H
#define STREAMER_H
#include "disk.h"

struct disk_stream
{
    int pos;
    struct disk* disk;
};

struct disk_stream* diskSteamerNew(int diskNo);
int diskSteamerSeek(struct disk_stream* stream, int pos);
int diskSteamerRead(struct disk_stream* stream, void* out, int total);
void diskStreamerClose(struct disk_stream* stream);

#endif