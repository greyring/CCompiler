#include "util.h"
#include "symbol.h"
#include "env.h"

E_envs E_base_envs(void)
{
    E_envs p = checked_malloc(sizeof(*p));
    p->tenv = S_empty(void);
    p->venv = S_empty(void);
    p->tagenv = s_empty(void);
    return p;
}

E_enventry E_varEntry(Ty_ty ty){
    E_enventry_ e;
    e.kind = E_varEntry;
    e.u.var.ty = ty;
    return &e;
}

E_enventry E_FunEntry(Ty_tyList formals, Ty_ty result){
    E_enventry_ e;
    e.kind = E_funEntry;
    e.u.fun.formals = formals;
    e.u.fun.result = result;
    return &e;
}

S_table E_base_tenv(void){
    return TAB_empty();
}

S_table E_base_venv(void){
    return TAB_empty();
}