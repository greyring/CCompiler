#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include "util.h"

#define S_SIZE 109

typedef struct S_symbol_ *S_symbol;

typedef struct TAB_table_ *S_table;

//symbol定义
struct S_symbol_ 
{
    string name; 
    S_symbol next;
};

//创建一个symbol，包装string
S_symbol _S_symbol(string name);

//创建一个新table
S_table S_empty(void);

//替换一个symbol，隐藏旧的symbol，支持撤销
void S_enter(S_table t, S_symbol sym, void *value);

//查找一个symbol
void *S_look(S_table t, S_symbol sym);

//开始新scope，即插入一个<mark>
void S_beginScope(S_table t);

//结束scope，撤销，不停调用pop直到遇到<mark>
void S_endScope(S_table t);

//根据symbol返回变量名id
string S_name(S_symbol sym);


#endif