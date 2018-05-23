#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#define S_SIZE 109

typedef struct S_symbol_ *S_symbol;
typedef struct TAB_table_ *S_table;

struct S_symbol_ 
{
    char *name; 
    S_symbol next;
};

S_symbol _S_symbol(char *s);
S_table S_empty(void);
void S_enter(S_table t, S_symbol sym, void *value);
void *S_look(S_table t, S_symbol sym);
void S_beginScope(S_table t);
void S_endScope(S_table t);


#endif