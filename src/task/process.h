#ifndef PROCESS_H
#define PROCESS_H

#include "stdint.h"
#include "config.h"

struct process
{
    // the process ID
    uint16_t id;

    char filename[PEACHOS_MAX_PATH_LENGTH];

    // the main process task
    struct task* task;

    // The memory (malloc) allocations of the pgogram
    void* allocations[PEACHOS_MAX_PROGRAM_ALLOCATIONS];

    // the physical pointer to the process memory
    void* ptr;

    // the physical pointer to the stack memory
    void* stack;

    // the size of data pointed by ptr
    uint32_t size;

    struct keyboard_buffer
    {
        char buffer[PEACHOS_KEYBOARD_BUFFER_SIZE];
        int tail;
        int head;
    }keyboard;
};

int process_load(const char* filename, struct process** process);
struct process* process_current();
struct process* process_get(int process_id);
int process_load_switch(const char* filename, struct process** process);
int process_switch(struct process* process);

#endif