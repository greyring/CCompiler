#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include "util.h"
#include "table.h"

#define S_SIZE 109

typedef struct S_symbol_ *S_symbol;
typedef struct S_table_ *S_table;

typedef enum
{
    S_FILE,
    S_FUNC,
    S_BLOCK,
    S_FUNCPRO,
    S_EXLINK,
    S_INLINK,
    S_NOLINK,
} S_scope;

struct S_symbol_
{
    string name;
    S_symbol next;
};

struct S_table_
{
    S_table parent;
    S_scope scope;
    TAB_table table;
};

S_symbol _S_symbol(string name);
S_table S_empty(S_scope);
void S_enter(S_table, S_symbol, void *value);
void *S_look(S_table, S_symbol);
S_table S_beginScope(S_scope, S_table);
S_table S_endScope(S_table);
string S_name(S_symbol sym);

#endif
