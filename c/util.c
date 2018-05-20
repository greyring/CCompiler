#include "util.h"
#include <stdio.h>

void * checked_malloc(size_t size)
{
    void * temp = malloc(size);
    if (!temp)
    {
        printf("ERROR: Malloc failed!\n");
        exit(-1);
    }
    return temp;
}