#ifndef _TABLE_H_
#define _TABLE_H_

#define TABSIZE 127 //怎么来的
typedef struct binder_ *binder;
typedef struct TAB_table_ *TAB_table;

struct TAB_table_
{
    binder table[TABSIZE];
    void *top;
};

//创建新table
TAB_table TAB_empty(void);

//用新的symbol覆盖旧的symbol
void TAB_enter(TAB_table t, void *key, void *value);

//查询
void *TAB_lookup(TAB_table t, void *key);

//弹出一个symbol，类似栈
void *TAB_pop(TAB_table t);

#endif