#include "symbol.h"
#include "util.h"
#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//give each string an address
static S_symbol hashtable[S_SIZE];

static S_symbol insert(string name, S_symbol next)
{
    S_symbol s=checked_malloc(sizeof(*s));
    s->name=name; 
    s->next=next;
    return s;
}

static unsigned int hash(char *s0)
{
    unsigned int h = 0;
    char *s;
    for(s = s0; *s; s++)
        h = h * 65599 + *s;
    return h;
}

//create one only if not exist
S_symbol _S_symbol(string name)
{
    int index;
    S_symbol syms, sym;

    assert(name);
    index = hash(name) % S_SIZE;
    syms = hashtable[index];
    for (sym = syms; sym; sym = sym->next)
        if (strcmp(sym->name, name) == 0)
            return sym;
    sym = insert(name, syms);
    hashtable[index] = sym;
    return sym;
}

S_table S_empty(S_scope scope)
{
    S_table p = checked_malloc(sizeof(*p));
    p->parent = NULL;
    p->scope = scope;
    p->table = TAB_empty();
    return p;
}

void S_enter(S_table t, S_symbol sym, void *value)
{
    TAB_enter(t->table, sym, value);
}

//look in all scopes
void *S_look(S_table t, S_symbol sym)
{
    void *temp;
    temp = TAB_look(t->table, sym);
    if (t)
        return temp;
    if (t->parent)
        return S_look(t->parent, sym);
    return NULL;
}

//check only in this scope
void *S_check(S_table t, S_symbol sym)
{
    return TAB_look(t->table, sym);
}

//enter a scope
S_table S_beginScope(S_scope scope, S_table t)
{
    S_table p = checked_malloc(sizeof(*p));
    p->parent = t;
    p->scope = scope;
    p->table = TAB_empty();
    return p;
}

//exit a scope
S_table S_endScope(S_table t)
{
    S_table temp = t->parent;
    TAB_delete(t->table);
    free(t);
    return temp;
}

string S_name(S_symbol sym)
{
    return sym->name;
}
