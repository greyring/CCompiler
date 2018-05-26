#ifndef _TABLE_H_
#define _TABLE_H_

#define TABSIZE 127
typedef struct binder_ *binder;
typedef struct TAB_table_ *TAB_table;

struct TAB_table_
{
    binder table[TABSIZE];
    void *top;
};

TAB_table TAB_empty(void);
void TAB_enter(TAB_table t, void *key, void *value);
void *TAB_lookup(TAB_table t, void *key);
void *TAB_pop(TAB_table t);

#endif