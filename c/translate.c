#include "util.h"
#include "translate.h"
#include "tree.h"
#include "frame.h"
#include "types.h"
#include "errormsg.h"
#include <stdlib.h>
#include <assert.h>

static Tr_exp Tr_Ex(T_exp ex);
static Tr_exp Tr_Nx(T_stm nx);
static Tr_exp Tr_Cx(patchList t, patchList f, T_stm stm);
static Tr_accessList makeFormalAccessList(Tr_level level);
static Tr_access Tr_frame_access(Tr_level level, F_access access);
static Tr_accessList Tr_AccessList(Tr_accessList list, Tr_access access);
static T_expList Tr_toExpList(Tr_exp expList);
static T_exp unEx(Tr_exp exp);
static T_stm unNx(Tr_exp exp);
static struct Cx unCx(Tr_exp exp);
static patchList PatchList(Temp_label *head, patchList tail);
static void doPatch(patchList pList, Temp_label label);//link a patchlist to label
static patchList joinPatch(patchList fList, patchList sList);//link two patchlists

static F_fragList FRAGLIST;

static T_exp unEx(Tr_exp e)
{
    switch (e->kind)
    {
        case Tr_ex:
            return e->u.ex;
        case Tr_nx:
            return T_Eseq(e->u.nx, T_Const(0));
        case Tr_cx:
        {
            Temp_temp r = Temp_newtemp();
			Temp_label t = Temp_newlabel(), f = Temp_newlabel();
			doPatch(e->u.cx.t, t);
			doPatch(e->u.cx.f, f);
			return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
				T_Eseq(e->u.cx.stm,
					T_Eseq(T_Label(f), 
						T_Eseq(T_Move(T_Temp(r), T_Const(0)),
							T_Eseq(T_Label(t), T_Temp(r))))));            
        }
        default:
            assert(0);
    }
    return NULL;
}

static T_stm unNx(Tr_exp e)
{
    switch(e->kind)
    {
        case Tr_ex:
            return T_Exp(e->u.ex);
        case Tr_nx:
            return e->u.nx;
        case Tr_cx:
        {
            Temp_temp r = Temp_newtemp();
			Temp_label t = Temp_newlabel(), f = Temp_newlabel();
			doPatch(e->u.cx.t, t);
			doPatch(e->u.cx.f, f);
			return T_Exp(T_Eseq(T_Move(T_Temp(r), T_Const(1)),
				T_Eseq(e->u.cx.stm,
					T_Eseq(T_Label(f), 
						T_Eseq(T_Move(T_Temp(r), T_Const(0)),
							T_Eseq(T_Label(t), T_Temp(r)))))));
        }
        default:
            assert(0);
    }
    return NULL;
}

static struct Cx unCx(Tr_exp e)
{
    switch(e->kind)
    {
        case Tr_ex:
        {
            struct Cx cx;
            cx.stm = T_Cjump(T_eq, e->u.ex, T_Const(0), NULL, NULL);
            cx.t = PatchList(&(cx.stm->u.CJUMP.f), NULL);
            cx.f = PatchList(&(cx.stm->u.CJUMP.t), NULL);
            return cx;
        }
        case Tr_nx:
            assert(0);
        case Tr_cx:
            return e->u.cx;
        default:
            assert(0);
    }
}


static patchList PatchList(Temp_label *head, patchList tail)
{
	patchList pList = checked_malloc(sizeof(*pList));
	pList->head = head;
	pList->tail = tail;
	return pList;
}

static void doPatch(patchList pList, Temp_label label)
{
	for (; pList; pList = pList->tail)
		*(pList->head) = label;
}

static patchList joinPatch(patchList fList, patchList sList)
{
	if (!fList) return sList;
	for (; fList->tail; fList = fList->tail)
		;
	fList->tail = sList;
	return fList;
}

static T_expList Tr_toExpList(Tr_exp expList)
{
    T_expList list;
    if (!expList) return NULL;
    list = Tr_toExpList(expList->next);
    return T_ExpList(unEx(expList), list);
}

static Tr_exp Tr_Ex(T_exp ex)
{
    Tr_exp p = checked_malloc(sizeof(*p));
    p->kind = Tr_ex;
    p->u.ex = ex;
    return p;
}

static Tr_exp Tr_Nx(T_stm nx)
{
    Tr_exp p = checked_malloc(sizeof(*p));
    p->kind = Tr_nx;
    p->u.nx = nx;
    return p;
}

static Tr_exp Tr_Cx(patchList t, patchList f, T_stm stm)
{
    Tr_exp p = checked_malloc(sizeof(*p));
    p->kind = Tr_cx;
    p->u.cx.t = t;
    p->u.cx.f = f;
    p->u.cx.stm = stm;
    return p;
}

Tr_expList Tr_ExpList(Tr_expList list, Tr_exp exp)
{
    if (list.head == NULL)
        list.head = list.tail = exp;
    else
        list.tail = list.tail->next = exp;
    return list;
}

static Tr_accessList Tr_AccessList(Tr_accessList list, Tr_access access)
{
    if (list.head == NULL)
        list.head = list.tail = access;
    else
        list.tail = list.tail->next = access;
    return list;
}

static Tr_accessList makeFormalAccessList(Tr_level level)
{
    Tr_accessList list;
    F_access Faccess = F_formals(level->frame).head;
    list.head = list.tail = NULL;

	for (; Faccess; Faccess = Faccess->next) {
		Tr_access access = Tr_frame_access(level, Faccess);
		list = Tr_AccessList(list, access);
	}
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
    level->functy = functy;
    if (functy)
        level->frame = F_newFrame(name, functy->u.funcTy.params);
    else
    {
        Ty_fieldList temp;
        temp.head = temp.tail = NULL;
        level->frame = F_newFrame(name, temp);
    }
	level->formals = makeFormalAccessList(level);
	return level;
}

static Tr_access Tr_frame_access(Tr_level level, F_access access)
{
    Tr_access p = checked_malloc(sizeof(*p));
    p->level = level;
    p->kind = Tr_Frame;
    p->u.Frame = access;
    p->next = NULL;
    return p;
}

Tr_access Tr_StrAccess(string s)
{
    Tr_access p = Tr_frame_access(Tr_Outermost(), F_allocStr(Tr_Outermost()->frame, s));
    Tr_Outermost()->formals = Tr_AccessList(Tr_Outermost()->formals, p);
    return p;
}

Tr_access Tr_GlobalAccess(Ty_ty type)
{
    Tr_access p;
    if (!Ty_isIntTy(type->size.ty) || !Ty_isBasicCTy(type->size.ty))
    {
        EM_error(0, "global variable cannot be variable size");
        return NULL;
    }
    p = Tr_frame_access(Tr_Outermost(), F_allocLocal(Tr_Outermost()->frame, 1, Tr_getIntConst(type->size.exp)));
    Tr_Outermost()->formals = Tr_AccessList(Tr_Outermost()->formals, p);
    return p;
}

Tr_access Tr_ConstAccess(int i)
{
    Tr_access p = checked_malloc(sizeof *p);
    p->level = Tr_Outermost();
    p->kind = Tr_Const;
    p->u.Const = i;
    p->next = NULL;
    Tr_Outermost()->formals = Tr_AccessList(Tr_Outermost()->formals, p);
    return p;
}

Tr_access Tr_ExternAccess(S_symbol sym)
{
    Tr_access p = checked_malloc(sizeof *p);
    p->level = Tr_Outermost();
    p->kind = Tr_Extern;
    p->u.Extern = sym;
    p->next = NULL;
    Tr_Outermost()->formals = Tr_AccessList(Tr_Outermost()->formals, p);
    return p;
}

Tr_access Tr_StackAccess(Tr_level level, Ty_ty type)
{
    Tr_access p;
    if (!Ty_isIntTy(type->size.ty) || !Ty_isBasicCTy(type->size.ty))//todo
    {
        EM_error(0, "variable size alloc is not supported");
        return NULL;
    }
    p = Tr_frame_access(level, F_allocLocal(level->frame, 1, Tr_getIntConst(type->size.exp)));
    level->formals = Tr_AccessList(level->formals, p);
    return p;
}








Tr_exp Tr_simpleVar(Tr_level level, Tr_access access)
{
    if (access->level == Tr_Outermost())
    {
        switch (access->kind)
        {
            case Tr_Frame:
                return Tr_Ex(F_Var(access->u.Frame, T_Temp(F_FP())));
            case Tr_Extern:
                return Tr_Ex(F_ExVar(access->u.Extern));
            case Tr_Const:
                return Tr_IntConst(access->u.Const);
            default:
                assert(0);
        }
    }
    else if (access->level == level)
    {
        return Tr_Ex(F_Var(access->u.Frame, T_Temp(F_FP())));
    }
    else
    {
        EM_error(0, "static link not support");//todo pos
        return NULL;
    }
    assert(0);
}

Tr_exp Tr_simpleFunc(Tr_level level)
{
    return Tr_Nx(T_Label(level->name));
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

Tr_exp Tr_CharConst(char c)
{
    return Tr_Ex(T_Const(c));
}

char Tr_getCharConst(Tr_exp charConst)
{
    assert(charConst->kind == Tr_ex);
    assert(charConst->u.ex->kind == T_CONST);
    return (char)charConst->u.ex->u.CONST;
}

Tr_exp Tr_DoubleConst(double d)
{
    EM_error(0, "double not support");
    return NULL;
}

double Tr_getDoubleConst(Tr_exp doubleConst)
{
    EM_error(0, "double not support");
    return 0;
}

Tr_exp Tr_strExp(string s)
{
    return Tr_simpleVar(Tr_Outermost(), Tr_StrAccess(s));
}

Tr_exp Tr_subExp(Tr_exp idExp, Tr_exp subExp, Tr_exp size)
{
    return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(idExp), unEx(subExp))));
}

Tr_exp Tr_funcCallExp(Tr_exp funcexp, Tr_expList expList)
{
    T_expList args = Tr_toExpList(expList.head);
    return Tr_Ex(T_Call(unEx(funcexp), args));
}

Tr_exp Tr_dotExp(Tr_exp structExp, int offset)
{
    return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(structExp), T_Const(offset))));
}

Tr_exp Tr_pointExp(Tr_exp pointExp, int offset)
{
    return Tr_Ex(T_Mem(T_Binop(T_plus, T_Mem(unEx(pointExp)), T_Const(offset))));
}

Tr_exp Tr_postppIntExp(Tr_exp intExp)
{
    Temp_temp t = Temp_newtemp();
    
    return Tr_Ex(T_Eseq(
                T_Seq(T_Move(T_Temp(t), unEx(intExp)),
                        T_Move(unEx(intExp), T_Binop(T_plus, unEx(intExp), T_Const(1)))
                ),
            T_Temp(t)
            )
    );
}

Tr_exp Tr_postppPointerExp(Tr_exp pointerExp, Tr_exp size)
{
    Temp_temp t = Temp_newtemp();
    return Tr_Ex(T_Eseq(
                T_Seq(T_Move(T_Temp(t), unEx(pointerExp)),
                        T_Move(unEx(pointerExp), T_Binop(T_plus, unEx(pointerExp), unEx(size)))
                ),
            T_Temp(t)
            )
    );
}

Tr_exp Tr_postmmIntExp(Tr_exp intExp)
{
    Temp_temp t = Temp_newtemp();
    
    return Tr_Ex(T_Eseq(
                T_Seq(T_Move(T_Temp(t), unEx(intExp)),
                        T_Move(unEx(intExp), T_Binop(T_minus, unEx(intExp), T_Const(1)))
                ),
            T_Temp(t)
            )
    );
}


Tr_exp Tr_postmmPointerExp(Tr_exp pointerExp, Tr_exp size)
{
    Temp_temp t = Temp_newtemp();
    return Tr_Ex(T_Eseq(
                T_Seq(T_Move(T_Temp(t), unEx(pointerExp)),
                        T_Move(unEx(pointerExp), T_Binop(T_minus, unEx(pointerExp), unEx(size)))
                ),
            T_Temp(t)
            )
    );
}

Tr_exp Tr_preppIntExp(Tr_exp intExp)
{

    return Tr_Ex(T_Eseq(
            T_Move(unEx(intExp), T_Binop(T_plus, unEx(intExp), T_Const(1))),
            unEx(intExp))
    );
}

Tr_exp Tr_preppPointerExp(Tr_exp pointerExp, Tr_exp size)
{
    return Tr_Ex(T_Eseq(
            T_Move(unEx(pointerExp), T_Binop(T_plus, unEx(pointerExp), unEx(size))),
            unEx(pointerExp))
    );
}

Tr_exp Tr_premmIntExp(Tr_exp intExp)
{
    return Tr_Ex(T_Eseq(
            T_Move(unEx(intExp), T_Binop(T_minus, unEx(intExp), T_Const(1))),
            unEx(intExp))
    );
}

Tr_exp Tr_premmPointerExp(Tr_exp pointerExp, Tr_exp size)
{
    return Tr_Ex(T_Eseq(
            T_Move(unEx(pointerExp), T_Binop(T_minus, unEx(pointerExp), unEx(size))),
            unEx(pointerExp))
    );
}

Tr_exp Tr_getPointer(Tr_exp memExp)
{
    T_exp exp = unEx(memExp);
    assert(exp->kind != T_MEM);
    return Tr_Ex(exp->u.MEM);
}

Tr_exp Tr_getMem(Tr_exp pointExp)
{
    return Tr_Ex(T_Mem(unEx(pointExp)));
}

Tr_exp Tr_notInt(Tr_exp exp)
{
    return Tr_Ex(T_Binop(T_xor, unEx(exp), T_Const(0)));
}

Tr_exp Tr_notConst(Tr_exp exp)
{
    return Tr_IntConst(~Tr_getIntConst(exp));
}

Tr_exp Tr_lognotInt(Tr_exp exp)
{
    T_stm cond = T_Cjump(T_ne, unEx(exp), T_Const(0), NULL, NULL);
    patchList t = PatchList(&cond->u.CJUMP.t, NULL);
    patchList f = PatchList(&cond->u.CJUMP.f, NULL);
    return Tr_Cx(t, f, cond);
}

Tr_exp Tr_lognotConst(Tr_exp exp)
{
    return Tr_lognotInt(exp);
}

Tr_exp Tr_sizeof(Ty_ty type)
{
    return type->size.exp;
}

Tr_exp Tr_CBinop(Tr_binop op, Tr_exp exp1, Tr_exp exp2)
{
    int a, b;
    a = Tr_getIntConst(exp1);
    b = Tr_getIntConst(exp2);
    switch(op)
    {
        case Tr_MULINT:
            return Tr_IntConst(a*b);
        case Tr_DIVINT:
            return Tr_IntConst(a/b);
        case Tr_MODINT: 
            return Tr_IntConst(a%b);
        case Tr_PLUSINT:
            return Tr_IntConst(a+b);
        case Tr_SUBINT:
            return Tr_IntConst(a-b);
        case Tr_XORINT:
            return Tr_IntConst(a^b);
        case Tr_ORINT:
            return Tr_IntConst(a|b);
        case Tr_ANDINT:
            return Tr_IntConst(a&b);
        case Tr_LSINT:
            return Tr_IntConst(a<<b);
        case Tr_RSINT:
            return Tr_IntConst(a>>b);
        default:
            assert(0);
    }
}

Tr_exp Tr_Binop(Tr_binop op, Tr_exp exp1, Tr_exp exp2)
{
    switch(op)
    {
        case Tr_MULINT:
            return Tr_Ex(T_Binop(T_mul, unEx(exp1), unEx(exp2)));
        case Tr_DIVINT:
            return Tr_Ex(T_Binop(T_div, unEx(exp1), unEx(exp2)));
        case Tr_MODINT: 
            return Tr_Ex(T_Binop(T_minus, unEx(exp1), T_Binop(T_mul, unEx(exp2), T_Binop(T_div, unEx(exp1), unEx(exp2)))));
        case Tr_PLUSINT:
            return Tr_Ex(T_Binop(T_plus, unEx(exp1), unEx(exp2)));
        case Tr_SUBINT:
            return Tr_Ex(T_Binop(T_minus, unEx(exp1), unEx(exp2)));
        case Tr_XORINT:
            return Tr_Ex(T_Binop(T_xor, unEx(exp1), unEx(exp2)));
        case Tr_ORINT:
            return Tr_Ex(T_Binop(T_or, unEx(exp1), unEx(exp2)));
        case Tr_ANDINT:
            return Tr_Ex(T_Binop(T_and, unEx(exp1), unEx(exp2)));
        case Tr_LSINT:
            return Tr_Ex(T_Binop(T_lshift, unEx(exp1), unEx(exp2)));
        case Tr_RSINT:
            return Tr_Ex(T_Binop(T_rshift, unEx(exp1), unEx(exp2)));
        default:
            assert(0);
    }
}

Tr_exp Tr_CRelop(Tr_relop op, Tr_exp exp1, Tr_exp exp2)
{
    return Tr_Relop(op, exp1, exp2);
}

Tr_exp Tr_Relop(Tr_relop op, Tr_exp exp1, Tr_exp exp2)
{
    if (op == Tr_LOGANDINT)
    {
        struct Cx cx1 = unCx(exp1);
        struct Cx cx2 = unCx(exp2);
        Temp_label t = Temp_newlabel();
        patchList newf = joinPatch(cx1.f, cx2.f);
        doPatch(cx1.t, t);
        return Tr_Cx(cx2.t, newf, T_Seq(cx1.stm, T_Seq(T_Label(t), cx2.stm)));
    }
    else if (op == Tr_LOGORINT)
    {
        struct Cx cx1 = unCx(exp1);
        struct Cx cx2 = unCx(exp2);
        Temp_label f = Temp_newlabel();
        patchList newt = joinPatch(cx1.t, cx2.t);
        doPatch(cx1.f, f);
        return Tr_Cx(newt, cx2.f, T_Seq(cx1.stm, T_Seq(T_Label(f), cx2.stm)));
    }

    Tr_relop rop;
    switch(op)
    {
        case Tr_GTINT: rop = T_gt; break;
        case Tr_GTEINT: rop = T_ge; break;
        case Tr_LTINT:  rop = T_lt; break;
        case Tr_LTEINT: rop = T_le; break;
        case Tr_EQINT:  rop = T_eq; break;
        case Tr_NEQINT: rop = T_ne; break;
        default:
            assert(0);
    }
    T_stm cond = T_Cjump(rop, unEx(exp1), unEx(exp2), NULL, NULL);
    patchList t = PatchList(&cond->u.CJUMP.t, NULL);
    patchList f = PatchList(&cond->u.CJUMP.f, NULL);
    return Tr_Cx(t, f, cond);
}

Tr_exp Tr_assignExp(Tr_exp var, Tr_exp exp)
{
    Temp_temp t = Temp_newtemp();
    return Tr_Ex(T_Eseq(T_Seq(T_Move(T_Temp(t), unEx(exp)), T_Move(unEx(var), T_Temp(t))), T_Temp(t)));
}

Tr_exp Tr_seqExp(Tr_exp exp1, Tr_exp exp2)
{
    return Tr_Ex(T_Eseq(unNx(exp1), unEx(exp2)));
}

Tr_exp Tr_seqStat(Tr_exp exp1, Tr_exp exp2)
{
    if (exp1 == NULL && exp2 == NULL)
        return NULL;
    if (exp1 == NULL)
        return exp2;
    if (exp2 == NULL)
        return exp1;
    return Tr_Nx(T_Seq(unNx(exp1), unNx(exp2)));
}

Tr_exp Tr_jumpStat(Temp_label label)
{
    return Tr_Nx(T_Jump(T_Name(label), Temp_LabelList(label, NULL)));
}

Tr_exp Tr_labelStat(Temp_label label)
{
    return Tr_Ex(T_Name(label));
}

Tr_exp Tr_iftStat(Tr_exp cond, Tr_exp then)
{
    struct Cx condCx = unCx(cond);
    Temp_label t = Temp_newlabel();
    Temp_label f = Temp_newlabel();
    T_stm res = NULL;
    
    doPatch(condCx.t, t);
    doPatch(condCx.f, f);
    res = T_Seq(T_Label(t), unNx(then));
    res = T_Seq(res, T_Label(f));
    res = T_Seq(condCx.stm, res);
    return Tr_Nx(res);
}

Tr_exp Tr_iftfStat(Tr_exp cond, Tr_exp then, Tr_exp els)
{
    struct Cx condCx = unCx(cond);
    Temp_label t = Temp_newlabel();
    Temp_label f = Temp_newlabel();
    Temp_label end = Temp_newlabel();
    T_stm thenStm = NULL;
    T_stm elseStm = NULL;
    T_stm res = NULL;
    
    doPatch(condCx.t, t);
    doPatch(condCx.f, f);
    thenStm = T_Jump(T_Name(end), Temp_LabelList(end, NULL));
    thenStm = T_Seq(unNx(then), thenStm);
    thenStm = T_Seq(T_Label(t), thenStm);
    elseStm = T_Jump(T_Name(end), Temp_LabelList(end, NULL));
    elseStm = T_Seq(unNx(els), elseStm);
    elseStm = T_Seq(T_Label(f), elseStm);
    res = T_Seq(thenStm, elseStm);
    res = T_Seq(res, T_Label(end));
    res = T_Seq(condCx.stm, res);
    return Tr_Nx(res);
}

Tr_exp Tr_whileStat(Temp_label test, Tr_exp cond, Tr_exp body, Temp_label done)
{
    Temp_label bodyStart = Temp_newlabel();
    struct Cx condCx = unCx(cond);
    T_stm res = NULL;

    doPatch(condCx.t, bodyStart);
    doPatch(condCx.f, done);
    res = T_Seq(unNx(body), T_Jump(T_Name(test), Temp_LabelList(test, NULL)));
    res = T_Seq(T_Label(bodyStart), res);
    res = T_Seq(condCx.stm, res);
    res = T_Seq(res, T_Label(done));
    res = T_Seq(T_Label(test), res);
    return Tr_Nx(res);
}

Tr_exp Tr_dowhileStat(Temp_label bodyStart, Tr_exp body, Tr_exp cond, Temp_label done)
{
    struct Cx condCx = unCx(cond);
    T_stm res = NULL;

    doPatch(condCx.t, bodyStart);
    doPatch(condCx.f, done);
    res = T_Seq(unNx(body), condCx.stm);
    res = T_Seq(res, T_Label(done));
    res = T_Seq(T_Label(bodyStart), res);
    return Tr_Nx(res);
}

Tr_exp Tr_forexpcondStat(Tr_exp exp1, Temp_label test, Tr_exp exp2, Tr_exp body, Tr_exp exp3, Temp_label done)
{
    Temp_label bodyStart = Temp_newlabel();
    struct Cx condCx = unCx(exp2);
    T_stm res = NULL;

    doPatch(condCx.t, bodyStart);
    doPatch(condCx.f, done);
    if (exp3)
        res = T_Seq(unNx(body), unNx(exp3));
    else
        res = unNx(body);
    res = T_Seq(res, T_Jump(T_Name(test), Temp_LabelList(test, NULL)));
    res = T_Seq(condCx.stm, res);
    res = T_Seq(T_Label(test), res);
    res = T_Seq(res, T_Label(done));
    if (exp1)
        res = T_Seq(unNx(exp1), res);
    return Tr_Nx(res);
}

Tr_exp Tr_forexpStat(Tr_exp exp1, Temp_label test, Tr_exp body, Tr_exp exp3, Temp_label done)
{
    T_stm res = NULL;
    if (exp3)
        res = T_Seq(unNx(body), unNx(exp3));
    else
        res = unNx(body);
    res = T_Seq(res, T_Jump(T_Name(test), Temp_LabelList(test, NULL)));
    res = T_Seq(T_Label(test), res);
    res = T_Seq(res, T_Label(done));
    if (exp1)
        res = T_Seq(unNx(exp1), res);
    return Tr_Nx(res);
}

Tr_exp Tr_returnStat(Tr_level level, Tr_exp returnExp)
{
    T_stm res = NULL;
    res = T_Move(T_Temp(F_V0()), unEx(returnExp));
    res = T_Seq(res, T_Jump(T_Name(level->frame->end), Temp_LabelList(level->frame->end, NULL)));
    return Tr_Nx(res);
}

void Tr_procEntryExit(Tr_level level, Tr_exp body)
{
    //end label of frame is not linked to body
	F_frag frag = F_ProcFrag(unNx(body), level->frame);
	FRAGLIST = F_FragList(FRAGLIST, frag);
}

F_fragList Tr_getResult(void)
{
	return FRAGLIST;
}

