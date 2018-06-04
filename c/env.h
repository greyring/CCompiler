#ifndef _ENV_H_
#define _ENV_H_

#include "types.h"
#include "temp.h"
#include "translate.h"

typedef struct E_namespace_ *E_namespace;
typedef struct E_linkage_ *E_linkage;
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
            Tr_access access;
            Ty_ty ty;
        }var;
        struct{
            Tr_level level;
			Ty_ty functy;
        }fun;
    }u;
};

//namespaces
struct E_namespace_
{
    S_table lenv;//label     symbol->Temp_label
    S_table tenv;//tag       symbol->type
    S_table venv;//ordinary  symbol->E_enventry
    S_table menv;//member    symbol->E_enventry
};

struct E_linkage_
{
    S_table exlink;//symbol->Tr_access
    S_table inlink;//symbol->Tr_access
    S_table nolink;//symbol->Tr_access
};

E_enventry E_VarEntry(Tr_access access, Ty_ty ty);
E_enventry E_FunEntry(Ty_ty functy, Tr_level level);

//init namespaces
E_namespace E_Namespace(void);
E_linkage E_Linkage(void);

E_namespace E_BeginScope(S_scope scope, E_namespace space);
E_namespace E_EndScope(E_namespace space);

#endif
