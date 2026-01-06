#include "task.h"
#include "kernel.h"
#include "status.h"
#include "memory/paging/paging.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "idt/idt.h"
#include "string/string.h"

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
    paging_switch(task->page_directory);
    return 0;
}

int task_page()
{
    user_registers();
    task_switch(current_task);
    return 0;
}

int task_page_task(struct task* task)
{
    user_registers();
    paging_switch(task->page_directory);
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

void task_save_state(struct task* task, struct interrupt_frame* frame)
{
    task->registers.ip = frame->ip;
    task->registers.cs = frame->cs;
    task->registers.flags = frame->flags;
    task->registers.esp = frame->esp;
    task->registers.ss = frame->ss;
    task->registers.ebp = frame->ebp;
    task->registers.ebx = frame->ebx;
    task->registers.ecx = frame->ecx;
    task->registers.edi = frame->edi;
    task->registers.edx = frame->edx;
    task->registers.esi = frame->esi;
}

int copy_string_from_task(struct task* task, void* virtual, void* phy, int max)
{
    if (max > PAGE_SIZE_IN_BYTES)
    {
        return -EINVARG;
    }

    int res = 0;

    char* tmp = kZalloc(max);
    // heap shared by kernel and task because of linear page mapping
    if(!tmp)
    {
        res = -ENOMEM;
        goto out;
    }

    uint32_t* task_directory = task->page_directory->directory_entry;
    uint32_t old_entry = paging_get(task_directory, tmp);
    paging_map(task->page_directory, tmp, tmp, PAGING_IS_WRITEABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);
    paging_switch(task->page_directory);

    // This assumes that tmp page mapping for kernel and task is same
    strncpy(tmp, virtual, max);
    kernel_page();

    res = paging_set(task_directory, tmp, (void*)old_entry);
    if (res < 0)
    {
        res = -EIO;
        goto out_free;
    }

    strncpy(phy, tmp, max);

out_free:
    kFree(tmp);
out:
    return res;
}

void task_current_save_state(struct interrupt_frame* frame)
{
    if (task_current() == 0)
    {
        panic("No current task to save\n");
    }

    struct task* task = task_current();
    task_save_state(task, frame);
}

void* task_get_stack_item(struct task* task, int index)
{
    void* result = 0;

    uint32_t* sp_ptr = (uint32_t*) task->registers.esp;

    // swith to the give tasks page, not the current task
    task_page_task(task);

    // first element from stack frame esp is ss segment register value right, No all these are pushed into kernel stack and not user stack
    result = (void*) sp_ptr[index];

    // switch back to kernel page
    kernel_page();

    return result;

}