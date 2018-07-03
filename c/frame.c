#include "util.h"
#include "frame.h"
#include "temp.h"
#include "tree.h"
#include "types.h"
#include <string.h>

#define REG(name, num) \
static Temp_temp name ;\
Temp_temp F_##name (void)\
{\
    if ( name == NULL )\
        name = Temp_reg(num);\
    return name ;\
}

REG(ZERO, 0);
REG(AT, 1);
REG(V0, 2);
REG(V1, 3);
REG(A0, 4);
REG(A1, 5);
REG(A2, 6);
REG(A3, 7);
REG(T0, 8);
REG(T1, 9);
REG(T2, 10);
REG(T3, 11);
REG(T4, 12);
REG(T5, 13);
REG(T6, 14);
REG(T7, 15);
REG(S0, 16);
REG(S1, 17);
REG(S2, 18);
REG(S3, 19);
REG(S4, 20);
REG(S5, 21);
REG(S6, 22);
REG(S7, 23);
REG(T8, 24);
REG(T9, 25);
REG(K0, 26);
REG(K1, 27);
REG(GP, 28);
REG(SP, 29);
REG(FP, 30);
REG(RA, 31);

static F_access InFrame(int offset, int size)
{
    F_access p = checked_malloc(sizeof(*p));
    p->kind = F_inFrame;
    p->u.frame.offset = offset;
    p->u.frame.size = size;
    return p;
}

static F_access InReg(Temp_temp reg)
{
    F_access p = checked_malloc(sizeof(*p));
    p->kind = F_inReg;
    p->u.reg = reg;
    return p;
}

static F_access InStr(int offset, string s)
{
    F_access p = checked_malloc(sizeof(*p));
    p->kind = F_inStr;
    p->u.str.offset = offset;
    p->u.str.size = strlen(s)+1;
    p->u.str.str = s;
    return p;
}

static F_accessList makeFormalAccessList(F_frame f, Ty_fieldList formals)
{
	Ty_field field = NULL;
	F_accessList accessList;
    accessList.head = accessList.tail = NULL;
    f->size = 0;
	for (field = formals.head; field; field = field->next) {
		F_access access = NULL;
        access = InFrame(-4 - 4 * f->size, 4);//todo
        f->size += 4;
        accessList = F_AccessList(accessList, access);
	}
	return accessList;
}

F_accessList F_AccessList(F_accessList list, F_access access)
{
    if (list.head == NULL)
        list.head = list.tail = access;
    else
        list.tail = list.tail->next = access;
    return list;
}

F_fragList F_FragList(F_fragList list, F_frag frag)
{
    if (list.head == NULL)
        list.head = list.tail = frag;
    else
        list.tail = list.tail->next = frag;
    return list;
}

F_frame F_newFrame(Temp_label name, Ty_fieldList formals)
{
    F_frame f = checked_malloc(sizeof(*f));
    f->name = name;
    f->locals = makeFormalAccessList(f, formals);
    f->end = Temp_newlabel();
    return f;
}

F_frag F_ProcFrag(T_stm body, F_frame frame)
{
    F_frag p = checked_malloc(sizeof(*p));
    p->body = body;
    p->frame = frame;
    p->next = NULL;
    return p;
}

F_access F_allocLocal(F_frame f, int escape, int align, int size)
{
    //todo not escape
    F_access access = NULL;
    if (f->size % align)
    {
        f->size = f->size - f->size % align + align;
    }
    access = InFrame(-4 - 4 * f->size, size);//todo
    f->size += size;
    f->locals = F_AccessList(f->locals, access);
    return access;
}

F_access F_allocStr(F_frame f, string s)
{
    //todo
    F_access access = NULL;
    access = InStr(f->size, s);
    f->size += strlen(s)+1;
    f->locals = F_AccessList(f->locals, access);
    return access;
}

F_accessList F_formals(F_frame frame)
{
    return frame->locals;
}

T_exp F_Var(F_access access, T_exp framePtr)
{
    if (access->kind == F_inFrame)
        return T_Mem(T_Binop(T_plus, framePtr, T_Const(access->u.frame.offset)));
    else if (access->kind == F_inStr)
        return T_Mem(T_Binop(T_plus, framePtr, T_Const(access->u.str.offset)));
    else
        return T_Temp(access->u.reg);
}

T_exp F_ExVar(S_symbol sym)
{
    //todo
    return NULL;
}

static Temp_tempList F_make_caller_saves(void)
{
    return Temp_TempList(F_V0(),
    Temp_TempList(F_V1(),
    Temp_TempList(F_A0(),
    Temp_TempList(F_A1(),
    Temp_TempList(F_A2(),
    Temp_TempList(F_A3(),
    Temp_TempList(F_S0(),
    Temp_TempList(F_S1(),
    Temp_TempList(F_S2(),
    Temp_TempList(F_S3(),
    Temp_TempList(F_S4(),
    Temp_TempList(F_S5(),
    Temp_TempList(F_S6(),
    Temp_TempList(F_S7(), NULL
    ))))))))))))));
}

static Temp_tempList F_make_calle_saves(void)
{
    return Temp_TempList(F_FP(),
    Temp_TempList(F_RA(), NULL
    ));
}

static Temp_tempList F_caller_saves(void)
{
    static Temp_tempList caller_saves = NULL;
	if (!caller_saves) {
		caller_saves = F_make_caller_saves();
	}
	return caller_saves;
}

static Temp_tempList F_callee_saves(void)
{
    static Temp_tempList callee_saves = NULL;
	if (!callee_saves) {
		callee_saves = F_make_calle_saves();
	}
	return callee_saves;
}

T_exp F_externalCall(string str, T_expList args);
