#ifndef INST_H
#define INST_H

#include "temp.h"
#include "tree.h"



typedef enum{
    I_ADD,
    I_SUB,
    I_MUL,
    I_DIV,
    I_AND,
    I_XOR,
    I_OR,
    I_SLLV,
    I_SRLV,
    I_SLTU
}iInst;

typedef enum{
    T_BEQ,
    T_BNE
}tbInst;

typedef enum{
    O_BGEZ,
    O_BGTZ,
    O_BLEZ,
    O_BLTZ
}obInst;

typedef enum{
    J_J,
    J_JAL
}jInst;

typedef enum{
    W_LW,
    W_SW
}wInst;

typedef T_inst_* T_inst;
typedef T_instList_* T_instList;
typedef T_instListList_* T_instListList;

struct T_inst_{
    enum{
        I_INST,
        T_INST,
        O_INST,
        J_INST,
        W_INST,
        M_INST,
        C_INST,
        L_INST,
        NULL_FOR_TEMP
    }kind;
    Temp_temp res;
    union{
        struct{
            iInst type;
            Temp_temp rs, rt, rd;
        }i;
        struct{
            tbInst type;
            Temp_temp rs, rt;
            Temp_label label;
        }t;
        struct{
            obInst type;
            Temp_temp rs;
            Temp_label label;
        }o;
        struct{
            jInst type;
            Temp_label label;
        }j;
        struct{
            wInst type;
            Temp_temp rs, rt;
            int num;
        }w;
        struct{
            Temp_temp rs, rt;
        }m;
        struct{

            Temp_temp rt;
            int num;
        }c;
        struct{
            Temp_label label;
        }l;
    }u;
};

struct T_instList_{
    T_inst head;
    T_instList tail;
};

struct T_instListList_{
    T_instList head;
    T_instListList tail;
};

T_inst T_inst_I(iInst type, Temp_temp rs, Temp_temp rt, Temp_temp rd);
T_inst T_inst_T(tbInst type, Temp_temp rs, Temp_temp rt, Temp_label label);
T_inst T_inst_O(obInst type, Temp_temp rs, Temp_label label);
T_inst T_inst_J(jInst type, Temp_label label);
T_inst T_inst_W(wInst type, Temp_temp rs, Temp_temp rt, int num);
T_inst T_inst_M(Temp_temp rs, Temp_temp rt);
T_inst T_inst_C(Temp_temp rs, int num);
T_inst T_inst_L(Temp_label label);
T_inst T_inst_N(Temp_temp res);


void T_instListAppend(T_instList f, T_instList s);
T_instList T_InstList(T_inst i);

void T_instListListAppend(T_instListList f, T_instListList s);
T_instListList T_InstListList(T_instList i);

T_instList T_makeExp(T_exp t);
T_instList T_makeStm(T_stm t);

T_instList T_makeExpList(T_expList t);
T_instList T_makeStmList(T_stmList t);

#endif