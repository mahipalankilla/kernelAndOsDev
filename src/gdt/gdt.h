#ifndef GDT_H
#define GDT_H

#include "stdint.h"

struct gdt
{
    uint16_t limit0_15;
    uint16_t base0_15;
    uint8_t base16_23;
    uint8_t access_byte;
    uint8_t flags; // this includes limit 16_19
    uint8_t base24_31;
}__attribute__((packed));

struct gdt_structured
{
    uint32_t base;
    uint32_t limit;
    uint8_t type;
};

void gdt_load(struct gdt* gdt, int size);
void gdt_structured_to_gdt(struct gdt* gdt, struct gdt_structured* gdt_structured, int total);

#endif