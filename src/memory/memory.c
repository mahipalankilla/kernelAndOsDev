#include <memory/memory.h>

void* memset(void* ptr, int t, size_t n)
{
    for(size_t i = 0; i < n ; i++)
    {
        *((char *)(ptr) + i) = (char)(t);
    }

    return ptr; 
}

int memcmp(void* s1, void* s2, int count)
{
    char* c1 = (char*)s1;
    char* c2 = (char*)s2;

    while(count-- > 0)
    {
        if (*c1++ != *c2++)
        {
            return c1[-1] < c2[-1] ? -1 : 1;
        }
    }

    return 0;
}