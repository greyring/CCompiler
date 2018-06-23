#include "util.h"
#include "symbol.h"
#include "env.h"
#include "translate.h"

E_namespace E_Namespace(void)
{
    E_namespace p = checked_malloc(sizeof(*p));
    p->lenv = NULL;
    p->tenv = S_empty(S_FILE);
    p->venv = S_empty(S_FILE);
    p->menv = NULL;
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

E_namespace E_BeginScope(S_scope scope, E_namespace space)
{
    //space->lenv = S_beginScope(scope, space->lenv);
    space->tenv = S_beginScope(scope, space->tenv);
    space->venv = S_beginScope(scope, space->venv);
    space->menv = S_beginScope(scope, space->menv);
    return space;
}

E_namespace E_EndScope(E_namespace space)
{
    //space->lenv = S_endScope(space->lenv);
    space->tenv = S_endScope(space->tenv);
    space->venv = S_endScope(space->venv);
    space->menv = S_endScope(space->menv);
    return space;
}

E_enventry E_VarEntry(Tr_access access, Ty_ty ty){
    E_enventry e = checked_malloc(sizeof(*e));
    e->kind = E_varEntry;
    e->u.var.access = access;
    e->u.var.ty = ty;
    return e;
}

E_enventry E_FuncEntry(Ty_ty functy, Tr_level level)
{
    E_enventry e = checked_malloc(sizeof(*e));
    e->kind = E_funcEntry;
    e->u.func.functy = functy;
    e->u.func.level = level;
    return e;
}

E_enventry E_LabelEntry(Temp_label label, int complete)
{
    E_enventry e = checked_malloc(sizeof(*e));
    e->kind = E_labelEntry;
    e->u.label.label = label;
    e->u.label.complete = complete;
    return e;
}

S_symbol E_checkLabel(S_table lenv)
{
    if (!lenv) return NULL;
    binder b = lenv->table->top;
    while(b)
    {
        if (((E_enventry)(b->value))->u.label.complete == 0)
            return b->key;
        b = b->next;
    }
    return NULL;
}
