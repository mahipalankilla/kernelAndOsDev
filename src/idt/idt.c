#include <idt/idt.h>
#include <config.h>
#include <memory/memory.h>
#include <kernel.h>

struct idt_desc idt_descriptor_table[PEACHOS_TOTAL_INTERRUPTS];
struct idtr_desc idtr;
extern void load_idt(struct idtr_desc *idtr);
void idt_init();
void interruptHandlerSample();
extern void problem();

void interruptHandlerSample()
{
    print("Interrupt Handler Called\n");
    return;
}

void setupInterruptHandler(void *ptr, int intID, int flags)
{
    struct idt_desc* idtEntryPtr = &idt_descriptor_table[intID];
    idtEntryPtr->offset_1 = (uint32_t)ptr & (0x0000FFFF);
    idtEntryPtr->selector = CODE_SELECTOR;
    idtEntryPtr->zero = 0;
    idtEntryPtr->type_attr = flags;
    idtEntryPtr->offset_2 = (uint32_t)ptr >> 16;
}

void idtInit()
{
    memset(idt_descriptor_table, 0, sizeof(idt_descriptor_table));
    idtr.limit = sizeof(idt_descriptor_table) - 1;
    idtr.base = (uint32_t)&idt_descriptor_table[0];
    load_idt(&idtr);
    setupInterruptHandler(interruptHandlerSample, 1, 0x8E);
    problem();
}