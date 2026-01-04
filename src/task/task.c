#include "task.h"
#include "kernel.h"
#include "status.h"
#include "memory/paging/paging.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"

// The current task that is running
struct task* current_task = 0;

// Task linked list
struct task* task_list_head = 0;
struct task* task_list_tail = 0;

struct task* task_current()
{
    return current_task;
}

static struct task* task_get_next()
{
    if (!current_task->next)
    {
        return task_list_head;
    }

    return current_task->next;
}

static void task_list_remove(struct task* task)
{
    if (task->prev)
    {
        task->prev->next = task->next;
        if (task->next)
        {
            task->next->prev = task->prev;
        }
    }

    if (task == task_list_head)
    {
        task_list_head = task->next;
    }

    if (task == task_list_tail)
    {
        task_list_tail = task->prev;
    }

    if (task == current_task)
    {
        current_task = task_get_next();
    }
}


int task_free(struct task* task)
{
    paging_free_4gb(task->page_directory);
    task_list_remove(task);

    // Finally free the task
    kFree(task);

    return 0;
}

int task_init(struct task* task, struct process* process)
{
    memset(task, 0, sizeof(struct task));

    // Map the entrie 4GB address space to itself
    task->page_directory = paging_new_4gb(PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    if(!task->page_directory)
    {
        return -EIO;
    }

    task->registers.ip = PEACHOS_PROGRAM_VIRTUAL_ADDRESS;
    task->registers.ss = USER_DATA_SEGMENT;
    task->registers.esp = PEACHOS_PROGRAM_STACK_VIRTUAL_ADDRESS_START;
    task->registers.cs = USER_CODE_SEGMENT;
    task->process = process;

    return 0;
}

struct task* task_new(struct process* process)
{
    int res = 0;
    struct task* task = kZalloc(sizeof(struct task));

    if (!task)
    {
        res = -ENOMEM;
        goto out;
    }

    res = task_init(task, process);
    if (res != PEACHOS_ALL_OK)
    {
        goto out;
    }

    if (task_list_head == 0)
    {
        task_list_head = task;
        task_list_tail = task;
        current_task = task;
        goto out;
    }

    task_list_tail->next = task;
    task->prev = task_list_tail;
    task_list_tail = task;

out: 
    if (ISERR(res))
    {
        task_free(task);
        return ERROR(res);
    }
    return task;
}

int task_switch(struct task* task)
{
    current_task = task;
    paging_switch(task->page_directory->directory_entry);
    return 0;
}

int task_page()
{
    user_registers();
    task_switch(current_task);
    return 0;
}

void task_run_first_ever_task()
{
    if(!current_task)
    {
        panic("task_run_first_ever_task(): No current task exits!\n");
    }

    task_switch(task_list_head);
    task_return(&task_list_head->registers);
}