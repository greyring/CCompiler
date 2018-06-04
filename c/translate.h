#ifndef _TRANSLATE_H_
#define _TRANSLATE_H_

#include "temp.h"
#include "frame.h"

typedef void *Tr_exp;
typedef void *Tr_level;
typedef void *Tr_access;//mem reg const

/*
typedef struct Tr_level_ *Tr_level;
typedef struct Tr_access_ *Tr_access;

struct Tr_level_
{
    Tr_level parent;
    Temp_label label;
    F_frame frame;
    Tr_access static_link;
    //Tr_accessList
};

//the abstruct structure for accessing a variable
struct Tr_access_
{
    Tr_level level;
    F_access access;
};

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
Tr_access Tr_ConstAccess(Tr_exp exp);

#endif