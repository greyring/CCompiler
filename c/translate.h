#ifndef _TRANSLATE_H_
#define _TRANSLATE_H_

#include "symbol.h"
#include "frame.h"
typedef struct Ty_ty_ *Ty_ty;

typedef struct patchList_ *patchList;
typedef struct Tr_exp_ *Tr_exp;
typedef struct Tr_expList_ Tr_expList;
typedef struct Tr_level_ *Tr_level;
typedef struct Tr_access_ *Tr_access;//mem reg const
typedef struct Tr_accessList_ Tr_accessList;

typedef enum
{
    Tr_MULINT, Tr_DIVINT, Tr_MODINT, 
    Tr_PLUSINT, Tr_SUBINT,
    Tr_XORINT, Tr_ORINT, Tr_ANDINT,
    Tr_LSINT, Tr_RSINT,
}Tr_binop;

typedef enum
{
    Tr_GTINT, Tr_GTEINT, Tr_LTINT, Tr_LTEINT,
    Tr_EQINT, Tr_NEQINT,
    Tr_LOGANDINT, Tr_LOGORINT,
}Tr_relop;

struct patchList_
{
    Temp_label *head;
    patchList tail;
};

struct Cx
{
    patchList t;
    patchList f;
    T_stm stm;
};

struct Tr_exp_
{
    enum
    {
        Tr_ex, Tr_nx, Tr_cx
    }kind;
    union
    {
        T_exp ex;
        T_stm nx;
        struct Cx cx;
    }u;

    Tr_exp next;
};

struct Tr_expList_
{
    Tr_exp head;
    Tr_exp tail;
};

//the abstruct structure for accessing a variable
struct Tr_access_
{
    Tr_level level;
    enum
    {
        Tr_Frame,
        Tr_Const,//for enum
        Tr_Extern,//for extern
    }kind;
    union
    {
        F_access Frame;
        int Const;
        S_symbol Extern;
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
    Temp_label name;
    F_frame frame;
    Tr_accessList formals;
    Ty_ty functy;
};

Tr_level Tr_Outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, Ty_ty functy);


Tr_access Tr_StrAccess(string s);//alloc
Tr_access Tr_GlobalAccess(Ty_ty type);//alloc
Tr_access Tr_ConstAccess(int i);//alloc
Tr_access Tr_ExternAccess(S_symbol sym);//alloc
Tr_access Tr_StackAccess(Tr_level level, Ty_ty type);//alloc

Tr_exp Tr_nil();
Tr_exp Tr_simpleVar(Tr_level level, Tr_access access);
Tr_exp Tr_varAddress(Tr_level level, Tr_access access);
Tr_exp Tr_simpleFunc(Tr_level level);
Tr_exp Tr_IntConst(int i);
int Tr_getIntConst(Tr_exp intConst);
Tr_exp Tr_CharConst(char c);
char Tr_getCharConst(Tr_exp charConst);
Tr_exp Tr_DoubleConst(double d);
double Tr_getDoubleConst(Tr_exp doubleConst);
Tr_exp Tr_strExp(string s);
Tr_exp Tr_subExp(Tr_exp idExp, Tr_exp subExp, Tr_exp size);
Tr_exp Tr_subExpAddr(Tr_exp idExp, Tr_exp subExp, Tr_exp size);
Tr_exp Tr_funcCallExp(Tr_exp funcexp, Tr_expList expList);
Tr_exp Tr_dotExp(Tr_exp structExp, int offset);
Tr_exp Tr_dotAddrExp(Tr_exp structExp, int offset);
Tr_exp Tr_pointExp(Tr_exp pointExp, int offset);
Tr_exp Tr_pointAddrExp(Tr_exp pointExp, int offset);
Tr_exp Tr_postppIntExp(Tr_exp intExp);
Tr_exp Tr_postppPointerExp(Tr_exp pointerExp, Tr_exp size);
Tr_exp Tr_postmmIntExp(Tr_exp intExp);
Tr_exp Tr_postmmPointerExp(Tr_exp pointerExp, Tr_exp size);
Tr_exp Tr_preppIntExp(Tr_exp intExp);
Tr_exp Tr_preppPointerExp(Tr_exp pointerExp, Tr_exp size);
Tr_exp Tr_premmIntExp(Tr_exp intExp);
Tr_exp Tr_premmPointerExp(Tr_exp pointerExp, Tr_exp size);
Tr_exp Tr_getPointer(Tr_exp memExp);
Tr_exp Tr_getMem(Tr_exp pointExp);
Tr_exp Tr_notInt(Tr_exp exp);
Tr_exp Tr_notConst(Tr_exp exp);
Tr_exp Tr_lognotInt(Tr_exp exp);
Tr_exp Tr_lognotConst(Tr_exp exp);
Tr_exp Tr_sizeof(Ty_ty type);
Tr_exp Tr_CBinop(Tr_binop op, Tr_exp exp1, Tr_exp exp2);
Tr_exp Tr_CRelop(Tr_relop op, Tr_exp exp1, Tr_exp exp2);
Tr_exp Tr_Binop(Tr_binop op, Tr_exp exp1, Tr_exp exp2);
Tr_exp Tr_Relop(Tr_relop op, Tr_exp exp1, Tr_exp exp2);
Tr_exp Tr_assignExp(Tr_exp var, Tr_exp exp);
Tr_exp Tr_seqExp(Tr_exp exp1, Tr_exp exp2);

Tr_expList Tr_ExpList(Tr_expList expList, Tr_exp exp);

Tr_exp Tr_seqStat(Tr_exp exp1, Tr_exp exp2);//handle null
Tr_exp Tr_jumpStat(Temp_label label);
Tr_exp Tr_labelStat(Temp_label label);
Tr_exp Tr_iftStat(Tr_exp cond, Tr_exp then);
Tr_exp Tr_iftfStat(Tr_exp cond, Tr_exp then, Tr_exp els);
Tr_exp Tr_whileStat(Temp_label test, Tr_exp cond, Tr_exp body, Temp_label done);
Tr_exp Tr_dowhileStat(Temp_label bodyStart, Tr_exp body, Tr_exp cond, Temp_label done);
Tr_exp Tr_forexpcondStat(Tr_exp exp1, Temp_label test, Tr_exp exp2, Tr_exp body, Tr_exp exp3, Temp_label done);
Tr_exp Tr_forexpStat(Tr_exp exp1, Temp_label test, Tr_exp body, Tr_exp exp3, Temp_label done);
Tr_exp Tr_returnStat(Tr_level level, Tr_exp returnExp);

void Tr_procEntryExit(Tr_level level, Tr_exp body);
F_fragList Tr_getResult(void);

#endif