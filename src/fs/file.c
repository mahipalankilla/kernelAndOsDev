#include "file.h"
#include "config.h"
#include "memory/memory.h"
#include "status.h"
#include "kernel.h"
#include "memory/heap/kheap.h"
#include "fat/fat16.h"
#include "disk/disk.h"

struct fileSystem* filesystems[PEACHOS_MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[PEACHOS_MAX_FILE_DESCRIPTORS];

static struct fileSystem** fs_get_free_filesystem()
{
    int i = 0;
    for (i=0; i<PEACHOS_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] == 0)
        {
            return &filesystems[i];
        }
    }

    return 0;
}

void fs_insert_filesystem(struct fileSystem* filesystem)
{
    struct fileSystem** fs;
    fs = fs_get_free_filesystem();
    if (!fs)
    {
        print("problem inserting filessytem");
        while(1);
    }

    *fs = filesystem;
}

static void fs_static_load()
{
    fs_insert_filesystem(fat16_init());
}

void fs_load()
{
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init()
{
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

static int file_new_descriptor(struct file_descriptor** desc_out)
{
    int res = -ENOMEM;
    for(int i = 0; i <  PEACHOS_MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == 0)
        {
            struct file_descriptor* desc = kZalloc(sizeof(struct file_descriptor));

            if (desc)
            {
                // file descrioptors start at 1
                desc->index = i + 1;
                file_descriptors[i] = desc;
                *desc_out = desc;
                res = 0;
            }

            break;

        }
    }

    return res;
}

static struct file_descriptor* file_get_descriptor(int fd)
{
    if (fd <= 0 || fd > PEACHOS_MAX_FILE_DESCRIPTORS)
    {
        return 0;
    }

    // descriptors start at 1
    int index = fd - 1;
    return file_descriptors[index];
}

struct fileSystem* fs_resolve(struct disk* disk)
{
    struct fileSystem* fs = 0;

    for (int i = 0; i < PEACHOS_MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
        {
            fs = filesystems[i];
            break;
        }

    }

    return fs;
}

FILE_MODE file_get_mode_by_string(const char* str)
{
    FILE_MODE mode = FILE_MODE_INVALID;
    if (strncmp(str, "r", 1) == 0)
    {
        mode = FILE_MODE_READ;
    }
    if (strncmp(str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }
    if (strncmp(str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }

    return mode;
}
int fopen(const char* filename, const char* mode_str)
{
    int res = PEACHOS_ALL_OK;

    struct path_root* path_root = pathparser_parse(filename, 0);
    if (!path_root || !path_root->first)
    {
        res = -EINVARG;
        goto out;
    }

    struct disk* disk = disk_get(path_root->driveNo);
    if (!disk)
    {
        res = -EIO;
        goto out;
    }

    if (!disk->filesystem)
    {
        res = -EIO;
        goto out;
    }

    FILE_MODE mode = file_get_mode_by_string(mode_str);
    if (mode == FILE_MODE_INVALID)
    {
        res = -EIO;
        goto out;       
    }

    void* descriptor_private_data = disk->filesystem->open(disk, path_root->first, mode);
    if(ISERR(descriptor_private_data))
    {
        res = ERROR_I(descriptor_private_data);
        goto out;
    }

    struct file_descriptor* desc;
    res = file_new_descriptor(&desc);
    if (res != 0)
    {
        res = -EIO;
        goto out;
    }

    desc->filesystem = disk->filesystem; // if descriptor has disk which has filesystem, why do we need filesystem again here?
    desc->privateInfo = descriptor_private_data;
    desc->disk = disk;

    res = desc->index;
out:
    if (res < PEACHOS_ALL_OK)
    {
        res = 0;
        pathparser_free(path_root);
    }
    return res;
}

int fread(void* ptr, uint32_t size, uint32_t nmemb, int fd)
{
    int res = 0;

    if (size == 0 || nmemb == 0 || fd < 1)
    {
        res = -EINVARG;
        goto out;
    }

    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc)
    {
        res = -EINVARG;
        goto out;
    }

    res = desc->filesystem->read(desc->disk, desc->privateInfo, size, nmemb, (char*) ptr);

out:
    return res;
}

int fseek(int fd, int offset, FILE_SEEK_MODE whence)
{
    int res = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc)
    {
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->seek(desc->privateInfo, offset, whence);

out:
    return res;
}

int fstat(int fd, struct file_stat* stat)
{
    int res = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc)
    {
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->stat(desc->disk, desc->privateInfo, stat);
out:
    return res;
}

static void file_free_descriptor(struct file_descriptor* desc)
{
    file_descriptors[desc->index - 1] = 0x00;
    kFree(desc);
}

int fclose(int fd)
{
    int res = 0;
    struct file_descriptor* desc = file_get_descriptor(fd);
    if (!desc)
    {
        res = -EIO;
        goto out;
    }

    res = desc->filesystem->close(desc->privateInfo);
    if (res == PEACHOS_ALL_OK)
    {
        file_free_descriptor(desc);
    }

out:
    return res;
}