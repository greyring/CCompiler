#include "util.h"
#include "translate.h"
#include "tree.h"
#include "frame.h"
#include "types.h"
#include <stdlib.h>

static Tr_exp Tr_Ex(T_exp ex);
static Tr_exp Tr_Nx(T_stm nx);
static Tr_exp Tr_Cx(patchList t, patchList f, T_stm stm);
static Tr_accessList makeFormalAccessList(Tr_level level);
static Tr_access Tr_frame_access(F_access access);
static Tr_accessList Tr_AccessList(Tr_accessList list, Tr_access access);

static Tr_exp Tr_Ex(T_exp ex)
{
    Tr_exp p = checked_malloc(*p);
    p->kind = Tr_ex;
    p->u.ex = ex;
    return p;
}

static Tr_exp Tr_Nx(T_stm nx)
{
    Tr_exp p = checked_malloc(*p);
    p->kind = Tr_nx;
    p->u.nx = nx;
    return p;
}

static Tr_exp Tr_Cx(patchList t, patchList f, T_stm stm)
{
    Tr_exp p = checked_malloc(*p);
    p->kind = Tr_cx;
    p->u.cx.t = t;
    p->u.cx.f = f;
    p->u.cx.stm = stm;
    return p;
}

static Tr_accessList Tr_AccessList(Tr_accessList list, Tr_access access)
{
    if (list.head == NULL)
        list.head = list.tail = access;
    else
        list.tail = list.tail->next = access;
    return list;
}

static Tr_level Tr_outermost = NULL;
Tr_level Tr_Outermost(void)
{
    if (!Tr_outermost)
        return Tr_newLevel(NULL, Temp_newlabel(), NULL);
    return Tr_outermost;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, Ty_ty functy)
{
    Tr_level level = checked_malloc(sizeof(*level));
	level->parent = parent;
	level->name = name;
	level->frame = F_newFrame(name, functy->u.funcTy.params);
	level->formals = makeFormalAccessList(level);
	return level;
}

Tr_exp Tr_IntConst(int i)
{
    return Tr_Ex(T_Const(i));
}

int Tr_getIntConst(Tr_exp intConst)
{
    assert(intConst->kind == Tr_ex);
    assert(intConst->u.ex->kind == T_CONST);
    return intConst->u.ex->u.CONST;
}

static Tr_access Tr_frame_access(F_access access)
{
    Tr_access p = checked_malloc(sizeof(*p));
    p->kind = Tr_Frame;
    p->u.Frame = access;
    p->next = NULL;
    return p;
}

Tr_access Tr_StrAccess(string s)
{
    return Tr_frame_access(F_allocStr(Tr_outermost->frame, s));
}


Tr_access Tr_GlobalAccess(Ty_ty type)
{
    if (!Ty_isIntConst(type->size.ty))
    {
        EM_error(0, "global variable cannot be variable size");
        return NULL;
    }
    return Tr_frame_access(F_allocLocal(Tr_Outermost()->frame, 1, Tr_getIntConst(type->size.exp)));
}

Tr_access Tr_ConstAccess(int i)
{
    Tr_access p = checked_malloc(sizeof *p);
    p->kind = Tr_Const;
    p->u.Const = i;
    p->next = NULL;
    return p;
}

Tr_access Tr_ExternAccess(S_symbol sym)
{
    Tr_access p = checked_malloc(sizeof *p);
    p->kind = Tr_Extern;
    p->u.Extern = sym;
    p->next = NULL;
    Tr_Outermost()->vars = Tr_AccessList(Tr_Outermost()->vars, p);
    return p;
}

Tr_access Tr_StackAccess(Tr_level level, Ty_ty type)
{
    if (!Ty_isIntConst(type->size.ty))//todo
    {
        EM_error(0, "variable size alloc is not supported");
        return NULL;
    }
    return Tr_fram_access(F_allocLocal(level->frame, 1, Tr_getIntConst(type->size.exp)));
}




Tr_exp Tr_simpleVar(Tr_level level, Tr_access access)
{
    if (access->level == Tr_Outermost())
    {

        return T_Binop(T_plus, Temp_FP(), )
    }

    if (access->level != level)
    {
        EM_error("static link not support");
        return NULL;
    }

}


