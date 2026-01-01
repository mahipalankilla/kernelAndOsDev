#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include <idt/idt.h>
#include <io/io.h>
#include <memory/heap/kheap.h>
#include <memory/paging/paging.h>
#include <disk/disk.h>
#include "string/string.h"
#include "fs/pparser.h"
#include "disk/streamer.h"
#include "fs/file.h"

uint16_t* video_mem = 0;
uint16_t terminal_row = 0;
uint16_t terminal_col = 0;

uint16_t terminal_make_char(char c, char colour)
{
    return (colour << 8) | c;
}

void terminal_putchar(int x, int y, char c, char colour)
{
    video_mem[(y * VGA_WIDTH) + x] = terminal_make_char(c, colour);
}

void terminal_writechar(char c, char colour)
{
    if (c == '\n')
    {
        terminal_row += 1;
        terminal_col = 0;
        return;
    }
    
    terminal_putchar(terminal_col, terminal_row, c, colour);
    terminal_col += 1;
    if (terminal_col >= VGA_WIDTH)
    {
        terminal_col = 0;
        terminal_row += 1;
    }
}
void terminal_initialize()
{
    video_mem = (uint16_t*)(0xB8000);
    terminal_row = 0;
    terminal_col = 0;
    for (int y = 0; y < VGA_HEIGHT; y++)
    {
        for (int x = 0; x < VGA_WIDTH; x++)
        {
            terminal_putchar(x, y, ' ', 0);
        }
    }   
}


void print(const char* str)
{
    size_t len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        terminal_writechar(str[i], 15);
    }
}

static struct paging_4gb_chunk* kernel_page_chunk = 0;

void panic(const char* msg)
{
    print(msg);
    while(1);
}

void kernel_main()
{
    terminal_initialize();
   // print("Hello world!\ntest\n");
   
    // Initialize the heap
    KheapInit();

    // Initialize filesystems
    fs_init();

    // Search and initialize the disks after the filesystems Init to resolve the filessytem in the disk
    disk_search_and_init();

    // Initialize interrupt descriptor table
    idtInit();

    // Before enabling pagin, allocations done are on physical level, so these allocations 
    // will become virtual address after enabling paging and may point to different physical
    // addresses. Need to keep out eye for these
    // char* ptr = kZalloc(10);

    kernel_page_chunk = paging_new_4gb(PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    paging_switch(paging_4gb_chunk_get_directory(kernel_page_chunk));

    // *(ptr) = 'A';
    // ptr[1] = 'B';
    // ptr[2] = '\n';

    // ptr2 pointing to virtual address 0x1000
    // char* ptr2 = (char*)0x1000;

    enable_paging();
    enable_interrupts();
    // Enable this to test paging functionality
    // modifyPageTableEntry(ptr2, (uint32_t)ptr);

    // print(ptr);
    // // if ptr and ptr2 are pointing to same physical location
    // ptr2[1] = 'M';
    
    // print(ptr2);

    // Test read block working, buff[511] should have 0xaa (booter signature)
    // char buff[512];
    // disk_read_block(disk_get(0), 0, 1, buff);

    // struct disk_stream* stream = diskSteamerNew(0);
    // diskSteamerSeek(stream, 0x201);
    // unsigned char c  = 0;
    // diskSteamerRead(stream, &c, 1);

    // Testing for fat16 filessytem
    // int fd = fopen("0:/hello.txt", "r");
    // if (fd)
    // {
    //     char readfile[80] = {0};
    //     fseek(fd,2,SEEK_CUR);
    //     fread(readfile, 51, 1, fd);
    //     print(readfile);
    //     struct file_stat fs;
    //     fstat(fd, &fs);
    //     fclose(fd);
    //     print("testing\n");
    // }
}