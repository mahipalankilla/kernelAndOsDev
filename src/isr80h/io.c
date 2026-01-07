#include "io.h"
#include "task/task.h"
#include "status.h"
#include "kernel.h"
#include "keyboard/keyboard.h"

void* isr80h_commad1_print(struct interrupt_frame* frame)
{
    void* user_space_msg_buffer = task_get_stack_item(task_current(), 0);
    char buff[PAGE_SIZE_IN_BYTES];
    int res = PEACHOS_ALL_OK;
    res = copy_string_from_task(task_current(), user_space_msg_buffer, buff, PAGE_SIZE_IN_BYTES);
    print(buff);
    return ERROR(res);
}

void* isr80h_command2_getkey(struct interrupt_frame* frame)
{
    return (void*)(int)keyboard_pop();
}

void* isr80h_command3_putchar(struct interrupt_frame* frame)
{
    char c = (char)(int)task_get_stack_item(task_current(), 0);
    terminal_writechar(c, 15);
    return 0;
}