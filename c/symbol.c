#include "symbol.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

S_symbol _S_symbol(char *s)
{
  S_symbol p = (S_symbol)checked_malloc(sizeof(*p));
  p->name = (char*)checked_malloc(sizeof(*s)+1);
  strcpy(p->name, s);
  p->next= NULL;
  return p;
}