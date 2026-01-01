#ifndef STRING_H
#define STRING_H
#include "stdbool.h"

bool isDigit(char c);
int toNumericDigit(char c);
int strlen(const char* ptr);
int strnlen(const char* ptr, int max);
char* strcpy(char* dest, const char* src);
int istrncmp(const char* s1, const char* s2, int n);
int strncmp(const char* s1, const char* s2, int lenToCompare);
int strnlen_terminator(const char* str, int max, char terminator);

#endif