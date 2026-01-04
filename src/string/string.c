#include "string.h"

int strlen(const char* ptr)
{
    int i = 0;
    while (*ptr != 0)
    {
        i++;
        ptr++;
    }

    return i;
}

int toNumericDigit(char c)
{
    return c - 48;
}

bool isDigit(char c)
{
    return ((c >= 48) && (c <= 57));
}

int strnlen(const char* ptr, int max)
{
    int i = 0;

    for(i = 0; i< max; i++)
    {
        if (ptr[i] == 0)
            break;
    }

    return i;
}

char* strcpy(char* dest, const char* src)
{
    char* tmp = dest;
    while(*src != 0)
    {
        *dest = *src;
        src += 1;
        dest += 1;
    }

    *dest = 0x00;

    return tmp;
}

char* strncpy(char* dest, const char* src, int n)
{
   int i = 0;
   for (i = 0; i < n - 1 ; i++)
   {
     if (src[i] == 0x0)
        break;
     
      dest[i] = src[i];
   }

    dest[i] = 0x00;

    return dest;
}

int strnlen_terminator(const char* str, int max, char terminator)
{
    int len = 0;
    for (int i=0; i < max; i++)
    {
        if (str[i] == '\0' || str[i] == terminator)
        {
            break;
        }
        len++;
    }

    return len;
}

char toLower(char s)
{
    if (s >= 65 && s <= 90)
    {
        return s += 32;
    }

    return s;
}
int istrncmp(const char* s1, const char* s2, int n)
{
    unsigned char u1, u2;
    while (n--)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2 && toLower(u1) != toLower(u2))
        {
            return u1 - u2;
        }

        if(u1 == '\0')
        {
            return 0;
        }
    }
    return 0;
}

int strncmp(const char* s1, const char* s2, int lenToCompare)
{
    while (lenToCompare--)
    {
        if (*s1 != *s2)
        {
            return *s1 - *s2;
        }

        if(*s1 == 0)
        {
            return 0;
        }

        s1++;
        s2++;
    }

    return 0;
}