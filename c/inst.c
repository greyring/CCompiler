#include "temp.h"
#include "inst.h"
#include "util.h"
#include "tree.h"

T_inst T_inst_I(iInst type, Temp_temp rs, Temp_temp rt, Temp_temp rd){
    T_inst t = (T_inst)checked_malloc(sizeof(T_inst_));
    t->kind = I_INST;
    t->u.i.type = type;
    t->u.i.rs = rs;
    t->u.i.rt = rt;
    t->u.i.rd = rd;

    t->res = rd;
}
T_inst T_inst_T(tbInst type, Temp_temp rs, Temp_temp rt, Temp_label label){
    T_inst t = (T_inst)checked_malloc(sizeof(T_inst_));
    t->kind = T_INST;
    t->u.t.type = type;
    t->u.t.rs = rs;
    t->u.t.rt = rt;
    t->u.r.label = label;

    t->res = NULL;
}
T_inst T_inst_O(obInst type, Temp_temp rs, Temp_label label){
    T_inst t = (T_inst)checked_malloc(sizeof(T_inst_));
    t->kind = O_INST;
    t->u.o.type = type;
    t->u.o.rs = rs;
    t->u.o.label = label;

    t->res = NULL;
}
T_inst T_inst_J(jInst type, Temp_label label){
    T_inst t = (T_inst)checked_malloc(sizeof(T_inst_));
    t->kind = J_INST;
    t->u.j.type = type;
    t->u.j.label = label;

    t->res = NULL;
}
T_inst T_inst_W(wInst type, Temp_temp rs, Temp_temp rt, int num){
    T_inst t = (T_inst)checked_malloc(sizeof(T_inst_));
    t->kind = W_INST;
    t->u.w.type = type;
    t->u.w.rs = rs;
    t->u.w.rt = rt;
    t->u.w.num = num;

    t->res = NULL;
    if (type == W_LW) t->res = rt;
}
T_inst T_inst_M(Temp_temp rs, Temp_temp rt){
    T_inst t = (T_inst)checked_malloc(sizeof(T_inst_));
    t->kind = M_INST;
    t->u.m.rs = rs;
    t->u.m.rt = rt;

    t->res = NULL;
}
T_inst T_inst_C(Temp_temp rs, int num){
    T_inst t = (T_inst)checked_malloc(sizeof(T_inst_));
    t->kind = C_INST;
    t->u.c.rs = rs;
    t->u.c.num = num;

    t->res = rs;
}
T_inst T_inst_L(Temp_label label){
    T_inst t = (T_inst)checked_malloc(sizeof(T_inst_));
    t->kind = L_INST;
    t->u.l.label = label;

    t->res = NULL;
}
T_inst T_inst_N(Temp_temp res){
    T_inst t = (T_inst)checked_malloc(sizeof(T_inst_));
    t->kind = NULL_FOR_TEMP;
    t->res = res;
}

void T_instListAppend(T_instList f, T_instList s){
    f->tail = s;
}

T_instList T_InstList(T_inst i){
    T_instList t = (T_instList)checked_malloc(sizeof(T_instList_));
    t->head = i;
    t->tail = NULL;
    return t;
}

void T_instListListAppend(T_instListList f, T_instListList s){
    if (f->head == NULL && f->tail == NULL){
        f->head = s->head;
        f->tail = s->tail;
    }
    else f->tail = s;
}

T_instListList T_InstListList(T_instList i){
    T_instListList = (T_instListList)checked_malloc(sizeof(T_instListList_));
    t->head = i;
    t->tail = NULL;
    return t;
}

T_instList T_makeExp(T_exp t){
    switch(t->kind){
        case T_BINOP:{
            T_instList leftT = T_makeExp(t->u.BINOP.left);
            T_instList righT = T_makeExp(t->u.BINOP.right);
            T_inst i;
            switch(t->u.BINOP.op){
                case T_plus:{
                    i = T_inst_I(I_ADD, leftT->head->res, righ->headT->res, Temp_newtemp());
                    break;
                }
                case T_minus:{
                    i = T_inst_I(I_SUB, leftT->head->res, righT->head->res, Temp_newtemp());
                    break;
                }
                case T_mul:{
                    i = T_inst_I(I_MUL, leftT->head->res, righT->head->res, Temp_newtemp());
                    break;
                }
                case T_div:{
                    i = T_inst_I(I_DIV, leftT->head->res, righT->head->res, Temp_newtemp());
                    break;
                }
                case T_and:{
                    i = T_inst_I(I_AND, leftT->head->res, righT->head->res, Temp_newtemp());
                    break;
                }
                case T_or:{
                    i = T_inst_I(I_OR, leftT->head->res, righT->head->res, Temp_newtemp());
                    break;
                }
                case T_lshift:{
                    i = T_inst_I(I_SLLV, leftT->head->res, righT->head->res, Temp_newtemp());
                    break;
                }
                case T_rshift:{
                    i = T_inst_I(I_SRLV, leftT->head->res, righT->head->res, Temp_newtemp());
                    break;
                }
                case T_xor:{
                    i = T_inst_I(I_XOR, leftT->head->res, righT->head->res, Temp_newtemp());
                    break;
                }
            }
            T_instListAppend(leftT, righT);
            T_instListAppend(leftT, T_InstList(i));
            return leftT;
        }
        case T_MEM:{
            T_instList ml = T_makeExp(t->u.MEM);
            T_inst i = T_inst_W(W_LW, ml->head->res, Temp_newtemp(), 0);
            T_instListAppend(ml, T_InstList(i));
            return ml;
        }
        case T_TEMP:{
            return T_InstList(T_inst_N(t->u.TEMP));
        }
        case T_CONST:{
            return T_InstList(T_inst_C(Temp_newtemp(), t->u.CONST));
        }
        case T_CALL:{//unfinished
            break;
        }
    }
}


T_instList T_makeStm(T_stm t){
    switch(t->kind){
        case T_LABEL:{
            return T_instList(T_inst_L(t->u.LABEL));
        }
        case T_JUMP:{//?????
            return NULL;
        }
        case T_CJUMP:{
            T_instList leftT = T_makeExp(t->u.CJUMP.left);
            T_instList righT = T_makeExp(t->u.CJUMP.right);
            T_instList bnc;
            switch (t->u.CJUMP.op){
                case T_eq:{
                    T_inst rel = T_inst_T(T_BEQ, leftT->head->res, righT->head->res, t->u.CJUMP.true);
                    T_inst jmp = T_inst_J(J_J, t->u.CJUMP.false);
                    bnc = T_InstList(rel);
                    T_instListAppend(bnc, T_InstList(jmp));
                    break;
                }
                case T_ne:{
                    T_inst rel = T_inst_T(T_BNE, leftT->head->res, righT->head->res, t->u.CJUMP.true);
                    T_inst jmp = T_inst_J(J_J, t->u.CJUMP.false);
                    bnc = T_InstList(rel);
                    T_instListAppend(bnc, T_InstList(jmp));
                    break;
                }
                case T_lt:{
                    T_inst prem = T_inst_I(I_SUB, lefT->head->res, righT->head->res, Temp_newtemp());
                    T_inst rel = T_inst_O(O_BLTZ, prem->res, t->u.CJUMP.true);
                    T_inst jmp = T_inst_J(J_J, t->u.CJUMP.false);
                    bnc = T_InstList(prem);
                    T_instListAppend(bnc, T_InstList(rel));
                    T_instListAppend(bnc, T_InstList(jmp));
                    break;
                }
                case T_gt:{
                    T_inst prem = T_inst_I(I_SUB, lefT->head->res, righT->head->res, Temp_newtemp());
                    T_inst rel = T_inst_O(O_BGTZ, prem->res, t->u.CJUMP.true);
                    T_inst jmp = T_inst_J(J_J, t->u.CJUMP.false);
                    bnc = T_InstList(prem);
                    T_instListAppend(bnc, T_InstList(rel));
                    T_instListAppend(bnc, T_InstList(jmp));
                    break;
                }
                case T_le:{
                    T_inst prem = T_inst_I(I_SUB, lefT->head->res, righT->head->res, Temp_newtemp());
                    T_inst rel = T_inst_O(O_BLEZ, prem->res, t->u.CJUMP.true);
                    T_inst jmp = T_inst_J(J_J, t->u.CJUMP.false);
                    bnc = T_InstList(prem);
                    T_instListAppend(bnc, T_InstList(rel));
                    T_instListAppend(bnc, T_InstList(jmp));
                    break;
                }
                case T_ge:{
                    T_inst prem = T_inst_I(I_SUB, lefT->head->res, righT->head->res, Temp_newtemp());
                    T_inst rel = T_inst_O(O_BGEZ, prem->res, t->u.CJUMP.true);
                    T_inst jmp = T_inst_J(J_J, t->u.CJUMP.false);
                    bnc = T_InstList(prem);
                    T_instListAppend(bnc, T_InstList(rel));
                    T_instListAppend(bnc, T_InstList(jmp));
                    break;
                }
            }


            T_instListAppend(leftT, righT);
            T_instListAppend(leftT, bnc);
            return leftT;
        }
        case T_MOVE:{
            if (t->u.MOVE.dst->kind == T_MEM){
                T_instList leftT = T_makeExp(t->u.MOVE.dst->u.MEM);
                T_instList righT = T_makeExp(t->u.MOVE.src);
                T_inst sw = T_inst_W(W_SW, leftT->res, righT->res, 0);
                T_instListAppend(leftT, righT);
                T_instListAppend(leftT, T_InstList(sw));
                return leftT;
            }
            else if (t->u.MOVE.dst->kind == T_TEMP){
                T_instList righT = T_makeExp(t->u.MOVE.src);
                T_inst mv = T_inst_M(righT->res, t->u.MOVE.dst->u.TEMP);
                T_instListAppend(righT, T_InstList(mv));
                return righT;
            }
        }
        case T_EXP:{
            return T_makeExp(t->u.EXP);
        }
    }
}

T_instListList T_makeExpList(T_expList t){
    T_instListList res = T_InstListList(NULL);
    while (t != NULL){
        T_instList i = T_makeExp(t->head);
        T_instListListAppend(res, T_InstListList(i));
        t = t->tail;
    }
    return res;
}
T_instListList T_makeStmList(T_stmList t){
    T_instListList res = T_InstListList(NULL);
    while (t != NULL){
        T_instList i = T_makeStm(t->head);
        T_instListListAppend(res, T_InstListList(i));
        t = t->tail;
    }
    return res;
}