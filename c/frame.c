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

REG(FP, 30);
REG(SP, 29);
REG(GP, 28);
REG(RA, 31);
REG(V0, 2);

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

static F_accessList makeFormalAccessList(F_frame f, Ty_fieldList formals)
{
	Ty_field field = NULL;
	F_accessList accessList;
    accessList.head = accessList.tail = NULL;
    f->size = 4;//aside space for ra
	for (field = formals.head; field; field = field->next) {
		F_access access = NULL;
        access = InFrame(f->size, 4);//todo
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
    access = InFrame(f->size, size);//todo
    f->size += size;
    f->locals = F_AccessList(f->locals, access);
    return access;
}

F_access F_allocStr(F_frame f, string s)
{
    //todo
    F_access access = NULL;
    access = InFrame(f->size, strlen(s));
    f->size += strlen(s);
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
    else
        return T_Temp(access->u.reg);
}

T_exp F_ExVar(S_symbol sym)
{
    //todo
    return NULL;
}

Temp_tempList F_caller_saves(void);
T_exp F_externalCall(string str, T_expList args);
