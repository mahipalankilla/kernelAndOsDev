#include "io.h"
#include "task/task.h"
#include "status.h"
#include "kernel.h"

void* isr80h_commad1_print(struct interrupt_frame* frame)
{
    void* user_space_msg_buffer = task_get_stack_item(task_current(), 0);
    char buff[PAGE_SIZE_IN_BYTES];
    int res = PEACHOS_ALL_OK;
    res = copy_string_from_task(task_current(), user_space_msg_buffer, buff, PAGE_SIZE_IN_BYTES);
    print(buff);
    return ERROR(res);
}