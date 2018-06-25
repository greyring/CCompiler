#ifndef _FRAME_H_
#define _FRAME_H_

#include "temp.h"
#include "util.h"
#include "tree.h"
typedef struct Ty_fieldList_ Ty_fieldList;//a struct

typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;
typedef struct F_accessList_ F_accessList;
typedef struct F_frag_ *F_frag;
typedef struct F_fragList_ F_fragList;

struct F_access_
{
    enum{
        F_inFrame,
        F_inReg
    }kind;
    union{
        struct{
            int offset;
            int size;
        }frame;
        Temp_temp reg;
    }u;
    F_access next;
};

struct F_accessList_ 
{
    F_access head;
    F_access tail;
};

struct F_frame_
{
    F_accessList locals;
    Temp_label   name;
    Temp_label   end;
    int          size;
};

struct F_frag_ {
    T_stm body;
    F_frame frame;
    F_frag next;
};

struct F_fragList_ {
    F_frag head;
    F_frag tail;
};

Temp_temp F_FP(void);
Temp_temp F_SP(void);
Temp_temp F_GP(void);
Temp_temp F_RA(void);
Temp_temp F_V0(void);


F_accessList F_AccessList(F_accessList list, F_access access);
F_fragList F_FragList(F_fragList list, F_frag frag);

F_frame F_newFrame(Temp_label name, Ty_fieldList formals);
F_frag F_ProcFrag(T_stm body, F_frame frame);
F_access F_allocLocal(F_frame f, int escape, int align, int size);
F_access F_allocStr(F_frame f, string s);

F_accessList F_formals(F_frame frame);

T_exp F_Var(F_access access, T_exp framePtr);
T_exp F_ExVar(S_symbol sym);

//Temp_tempList F_caller_saves(void);
//T_exp F_externalCall(string str, T_expList args);

#endif