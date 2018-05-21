#ifndef _SYMBOL_H_
#define _SYMBOL_H_

typedef struct S_symbol_ *S_symbol;

S_symbol _S_symbol(char *s);

struct S_symbol_ {char *name; S_symbol next;};

#endif