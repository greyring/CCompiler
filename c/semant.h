#ifndef _SEMANT_H_
#define _SEMANT_H_

#include "types.h"

typedef struct expty_ *expty;

struct expty_
{
    Tr_exp exp;
    Ty_ty   ty;
};

/********************************
建立一个expty
Tr_exp来自translate模块，课本第7章，翻译成中间代码
暂时使用void*代替，完成translate模块后删除那个typedef
*********************************/
expty 
expTy(Tr_exp exp, Ty_ty ty);


//变量的检查
struct expty_
transVar();

//表达式的检查
struct expty_
transExp();


//声明检查
void
transDec();


//类型检查
struct Ty_ty_
transTy();


#endif