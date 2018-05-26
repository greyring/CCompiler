#ifndef _ENV_H_
#define _ENV_H_

#include "types.h"

typedef struct E_envs_ *E_envs;
typedef struct E_enventry_ *E_enventry;

struct E_enventry_
{
    enum{
        E_varEntry,
        E_funEntry
    }kind;
    union
    {
        struct{
            Ty_ty ty;
        }var;
        struct{
            Ty_tylist formals;
            Ty_ty result;
        }fun;
    }u;
};

struct E_envs_ //这是干嘛的
{
    S_table tenv;
    S_table venv;
    S_table tagenv; //这是干嘛的
};

//建立一个varEntry
E_enventry E_VarEntry(Ty_ty ty);
//建立一个funEntry
E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result);


//建立一个envs
E_envs E_base_envs(void);


//建立type环境
S_table E_base_tenv(void);
//建立value环境
S_table E_base_venv(void);

#endif