#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdlib.h>
#include "limits.h"

typedef char* string;
typedef int bool;
void * checked_malloc(size_t size);

string String(char* s);

#define TRUE 1
#define FALSE 0

#endif