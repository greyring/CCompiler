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

struct E_envs_
{
    S_table tenv;
    S_table venv;
    S_table tagenv;
};

E_enventry E_varEntry(Ty_ty ty);
E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result);

E_envs E_base_envs(void);

S_table E_base_tenv(void);
S_table E_base_venv(void);

#endif