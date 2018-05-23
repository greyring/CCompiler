#include "util.h"
#include <stdio.h>
#include <assert.h>

void * checked_malloc(size_t size)
{
    void * temp = malloc(size);
    assert(temp);
    return temp;
}