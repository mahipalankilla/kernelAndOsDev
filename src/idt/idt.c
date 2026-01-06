#include <idt/idt.h>
#include <config.h>
#include <memory/memory.h>
#include <kernel.h>
#include <io/io.h>
#include "task/task.h"

struct idt_desc idt_descriptor_table[PEACHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr;
extern void load_idt(struct idtr_desc *idtr);
void idt_init();
void interruptHandlerSample();
extern void problem();
extern void int21h();
extern void no_interrupt();
extern void isr80h_wrapper();

static ISR80H_COMMAND isr80h_commands[PEACHOS_MAX_ISR80H_COMMANDS];

void int21h_handler()
{
    print("keyboard pressed! \n");
    outb(0x20, 0x20);
}

void no_interrupt_handler()
{
    outb(0x20, 0x20);
}


void setupInterruptHandler(int intID, void *ptr)
{
    struct idt_desc* idtEntryPtr = &idt_descriptor_table[intID];
    idtEntryPtr->offset_1 = (uint32_t)ptr & (0x0000FFFF);
    idtEntryPtr->selector = KERNEL_CODE_SELECTOR;
    idtEntryPtr->zero = 0;
    idtEntryPtr->type_attr = 0xEE;
    idtEntryPtr->offset_2 = (uint32_t)ptr >> 16;
}

void idtInit()
{
    memset(idt_descriptor_table, 0, sizeof(idt_descriptor_table));
    idtr.limit = sizeof(idt_descriptor_table) - 1;
    idtr.base = (uint32_t)&idt_descriptor_table[0];

    for (int i=0; i < PEACHOS_TOTAL_INTERRUPTS; i++)
    {
        setupInterruptHandler(i, no_interrupt);
    }

    setupInterruptHandler(0x21, int21h);
    setupInterruptHandler(0x80,isr80h_wrapper);
    load_idt(&idtr);
}

void isr80h_register_command(int command_id, ISR80H_COMMAND func)
{
    if (command_id <0 || command_id >= PEACHOS_MAX_ISR80H_COMMANDS)
    {
        panic("the command is out ouf bounds\n");
    }

    if (isr80h_commands[command_id])
    {
        panic("you are attempting to overwrite an existing command\n");
    }

    isr80h_commands[command_id] = func;
}

void* isr80h_handle_command(int command, struct interrupt_frame* frame)
{
    void* result = 0;

    if(command <0 || command >= PEACHOS_MAX_ISR80H_COMMANDS)
    {
        // Invalid command
        return 0;
    }

    ISR80H_COMMAND command_func = isr80h_commands[command];
    if (!command_func)
    {
        return 0;
    }

    result = command_func(frame);

    return result;
}

void* isr80h_handler(int command, struct interrupt_frame* frame)
{
    void* res = 0;
    kernel_page();

    task_current_save_state(frame);
    res = isr80h_handle_command(command, frame);
    task_page();
    return res;
}