#include <memory/memory.h>

void* memset(void* ptr, int t, size_t n)
{
    for(size_t i = 0; i < n ; i++)
    {
        *((char *)(ptr) + i) = (char)(t);
    }

    return ptr; 
}