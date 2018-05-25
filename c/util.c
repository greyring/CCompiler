#include "util.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

string String(char* s)
{
    char *p = checked_malloc(sizeof(char) * (strlen(s) + 1));
    strcpy(p, s);
    return p;
}

void * checked_malloc(size_t size)
{
    void * temp = malloc(size);
    assert(temp);
    return temp;
}