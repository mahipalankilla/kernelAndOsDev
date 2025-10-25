#ifndef KHEAP_H
#define KHEAP_H

#include "stdint.h"
#include "stddef.h"

void KheapInit(void);

void* kMalloc(size_t bytes);
void kFree(void*);
void* kZalloc(size_t bytes);

#endif