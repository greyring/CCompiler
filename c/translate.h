#ifndef _TRANSLATE_H_
#define _TRANSLATE_H_

#include "symbol.h"
#include "frame.h"
typedef struct Ty_ty_ *Ty_ty;

typedef void *Tr_exp;
typedef struct Tr_level_ *Tr_level;
typedef struct Tr_access_ *Tr_access;//mem reg const
typedef struct Tr_accessList_ Tr_accessList;

//the abstruct structure for accessing a variable
struct Tr_access_
{
    Tr_level level;
    enum
    {
        Tr_Reg,
        Tr_Mem,
        Tr_Const
    }kind;
    union
    {
        struct
        {
            F_access access;
            Tr_exp init;
        }Reg, Mem;
    }u;

    Tr_access next;
};

struct Tr_accessList_
{
    Tr_access head;
    Tr_access tail;
};

struct Tr_level_
{
    Tr_level parent;
    Temp_label label;
    F_frame frame;
    Tr_accessList vars;
};




/*
Tr_exp Tr_intExp(int i)
{
    return Tr_Ex(T_INTCONST(i));
}

Tr_exp Tr_floatExp(double d)
{
    return Tr_Ex(T_DOUBLECONST(d));
}

Tr_exp Tr_charExp(char c)
{
    return Tr_Ex(T_CHARCONST(c));
}
*/

Tr_exp Tr_IntConst(int i);
int Tr_getIntConst(Tr_exp intConst);

Tr_access Tr_GlobalAccess(Ty_ty type);
Tr_access Tr_ConstAccess(Tr_exp exp);
Tr_access Tr_ExternAccess(S_symbol sym);
Tr_access Tr_StackAccess(Tr_level level, Ty_ty type);

Tr_exp Tr_assignExp(Tr_exp var, Tr_exp exp);
Tr_exp Tr_simpleVar(Tr_level level, Tr_access access);

void Tr_addInit(Tr_level, Tr_exp exp);

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, Ty_ty functy);
Tr_level Tr_endLevel(Tr_level level);

#endif