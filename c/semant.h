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


//A_exp语义分析
struct expty_
transExp();

//A_spec语义分析
int 
noStoreType(A_spec a, A_storage_type type);


//A_type语义分析
struct expty_
transType();


//A_pointer语义分析
struct expty_
transPointer();


//A_param语义分析
struct expty_
transParam();



//A_dec语义分析
struct expty_
transDec();

//A_declaration语义分析
struct expty_
transDeclaration();

//A_type_name语义分析
struct expty_
transTypeName();

//A_designator语义分析
struct expty_
transDesignator();

//A_init语义分析
struct expty_
transInit();

//A_stat语义分析
struct expty_
transStat();

//A_def语义分析
F_fragList 
transDef(A_def a);


#endif