#include "util.h"
#include "table.h"
#include <stdlib.h>
#include <assert.h>

static binder Binder(void *key, void *value, binder next, void *prevtop)
{
    binder b = checked_malloc(sizeof(*b));
    b->key = key;
    b->value = value;
    b->next = next;
    b->prevtop = prevtop;
    return b;
}

TAB_table TAB_empty(void)
{
    TAB_table t = checked_malloc(sizeof(*t));
    t->top = NULL;
    for (int i = 0; i < TABSIZE; i++)
    {
        t->table[i] = NULL;
    }
    return t;
}

void TAB_enter(TAB_table t, void *key, void *value)
{
    int index;
    assert(t && key);
    index = ((unsigned long)key) % TABSIZE;
    t->table[index] = Binder(key, value, t->table[index], t->top);
    t->top = key;
}

void *TAB_look(TAB_table t, void *key)
{
    int index;
    binder b;
    assert(t && key);
    index = ((unsigned long) key) % TABSIZE;
    for (b = t->table[index]; b; b = b->next)
        if (b->key == key) return b->value;
    return NULL;
}

void *TAB_pop(TAB_table t)
{
    void *k;
    binder b;
    int index;
    assert(t);
    k = t->top;
    assert(k);
    index = ((unsigned long)k) % TABSIZE;
    b = t->table[index];
    assert(b);
    t->table[index] = b->next;
    t->top = b->prevtop;
    return b->key;
}

void TAB_delete(TAB_table t)
{
    int i;
    binder temp, nxt;
    for (i = 0; i<127; i++)
    {
        temp = t->table[i];
        while(temp)
        {
            nxt = temp->next;
            free(temp);
            temp = nxt;
        }
    }
    free(t);
}



