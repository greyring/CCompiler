#include "util.h"
#include "symbol.h"
#include "env.h"

E_namespace E_Namespace(void)
{
    E_namespace p = checked_malloc(sizeof(*p));
    p->lenv = NULL;
    p->tenv = S_empty(S_FILE);
    p->venv = S_empty(S_FILE);
    //p->menv = NULL;
    return p;
}

E_linkage E_Linkage(void)
{
    E_linkage p = checked_malloc(sizeof(*p));
    p->exlink = S_empty(S_EXLINK);
    p->inlink = S_empty(S_INLINK);
    p->nolink = NULL;
    return p;
}

/*
E_enventry E_VarEntry(Ty_ty ty){
    E_enventry e = checked_malloc(sizeof(*e));
    e->kind = E_varEntry;
    e->u.var.ty = ty;
    return e;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result){
    E_enventry e = checked_malloc(sizeof(*e));
    e->kind = E_funEntry;
    e->u.fun.formals = formals;
    e->u.fun.result = result;
    return e;
}
*/
