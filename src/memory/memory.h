#ifndef MEMORY_H
#define MEMORY_H
#include <stddef.h>

void* memset(void* ptr, int t, size_t size);
int memcmp(void* s1, void* s2, int count);

#endif // MEMORY_H