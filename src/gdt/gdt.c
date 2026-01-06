#include "gdt.h"
#include "kernel.h"
#include "task/task.h"

void generate_gdt(uint8_t* gdt, struct gdt_structured source)
{
    // what is this condition is for? do we want to limit the segment to 16MB in case it's more than 64kB?
    if ((source.limit > 65536) && ((source.limit & 0xFFF) != 0xFFF))
    {
        panic("generate_gdt: Invalid argument\n");
    }

    gdt[6] = 0x40; // Indicates the limit is in bytes
    if (source.limit > 65536)
    {
        source.limit = source.limit >> 12;
        gdt[6] = 0xc0; // Indicates the limit is in 4KiB blocks
    }


    // Encode the limit
    gdt[0] = source.limit & 0xFF;
    gdt[1] = (source.limit >> 8) & 0xFF;
    gdt[6] |= ((source.limit >> 16) & 0x0F);

    // Encode the base
    gdt[2] = source.base & 0xFF;
    gdt[3] = (source.base >> 8) & 0xFF;
    gdt[4] = (source.base >> 16) & 0xFF;
    gdt[7] = (source.base >> 24) & 0xFF;

    // Set the access byte
    gdt[5] = source.type;

}

void gdt_structured_to_gdt(struct gdt* gdt, struct gdt_structured* gdt_structured, int total)
{
    // first gdt entry should be NULL
    for (int i = 1; i < total; i++)
    {
        generate_gdt((uint8_t*)&gdt[i], gdt_structured[i]);
    }
    return;
}