#include "symbol.h"
#include "util.h"
#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static S_symbol hashtable[S_SIZE];//give each string an address
static struct S_symbol_ marksym = {"<mark>", 0};

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

//提供string，如果散列表里存在就返回这个symbol，如果不存在，就建立这个symbol再返回它
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
    return sym;
}

//建新的table
S_table S_empty(void)
{
  return TAB_empty();
}

//替换一个symbol，隐藏旧的symbol，便于撤销操作
void S_enter(S_table t, S_symbol sym, void *value)
{
    TAB_enter(t, sym, value);
}

//查找
void *S_look(S_table t, S_symbol sym)
{
    return TAB_lookup(t, sym);
}

//开始新scope，在散列表的头部增加一个<mark>标志，作为scope开始标志，便于撤销操作
void S_beginScope(S_table t)
{
    S_enter(t, &marksym, NULL);
}

//取消scope，pop直到遇到<mark>为止
void S_endScope(S_table t)
{
    S_symbol s;
    do{
        s = TAB_pop(t);
    }while(s != &marksym);
}

//通过symbol查询变量名id
string S_name(S_symbol sym)
{
 return sym->name;
}
