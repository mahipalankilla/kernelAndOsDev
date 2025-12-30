#include <idt/idt.h>
#include <config.h>
#include <memory/memory.h>
#include <kernel.h>
#include <io/io.h>

struct idt_desc idt_descriptor_table[PEACHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr;
extern void load_idt(struct idtr_desc *idtr);
void idt_init();
void interruptHandlerSample();
extern void problem();
extern void int21h();
extern void no_interrupt();

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
    idtEntryPtr->selector = CODE_SELECTOR;
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
    load_idt(&idtr);
}