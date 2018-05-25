#ifndef TIGER_TREE_H_
#define TIGER_TREE_H_

#include "temp.h"

typedef struct T_stm_ *T_stm;
typedef struct T_exp_ *T_exp;
// typedef struct T_expList_ *T_expList;
// typedef struct T_stmList_ *T_stmList;

typedef enum 
{
    T_PLUS, 
    T_MINUS, 
    T_MUL, 
    T_DIV,
    T_AND, T_OR, T_XOR
    T_LSH, T_RSH, 
}T_binOp;

typedef enum  
{
    T_EQ, T_NE, T_LT, T_GT, T_LE, T_GE,
}T_relOp;

struct T_stm_{
	enum{
        T_seq_stm, 
        T_label_stm, 
        T_jump_stm, 
        T_cjump_stm, 
        T_move_stm, 
        T_exp_stm
    }kind;
	union{
		struct{
            T_stm stm, next;
        }seq;
		Temp_label label;
		struct{
            T_exp exp; 
            Temp_labelList jumps;
        }jump;
		struct{
			T_relOp op;
			T_exp left, right;
			Temp_label t, f;
		}cjump;
		struct{
            T_exp dst, src;
        }move;
		T_exp exp;
	} u;
};

struct T_exp_{
	enum{
        T_binop_exp, 
        T_mem_exp, 
        T_temp_exp, 
        T_eseq_exp, 
        T_name_exp, 
        T_constexp_exp, 
        T_call_exp
    }kind;
	union{
		struct{
            T_binOp op; 
            T_exp left, right;
        }binop;
		T_exp mem;
		Temp_temp temp;
		struct{
            T_stm stm; 
            T_exp exp;
        }eseq;
		Temp_label name;
		int constexp;
		struct{
            T_exp fun; 
            T_expList args;
        }call;
	}u;
};

// struct T_expList_{T_exp exp; T_expList next;};
// struct T_stmList_{T_stm stm; T_stmList next;};

// T_expList T_ExpList(T_exp head, T_expList tail);
// T_stmList T_StmList(T_stm head, T_stmList tail);

T_stm _T_seq_stm(T_stm, T_stm);
T_stm _T_label_stm(Temp_label);
T_stm T_Jump(T_exp exp, Temp_labelList labels);
T_stm T_Cjump(T_relOp op, T_exp left, T_exp right, Temp_label true, Temp_label false);
T_stm T_Move(T_exp, T_exp);
T_stm T_Exp(T_exp);

T_exp T_Binop(T_binOp, T_exp, T_exp);
T_exp T_Mem(T_exp);
T_exp T_Temp(Temp_temp);
T_exp T_Eseq(T_stm, T_exp);
T_exp T_Name(Temp_label);
T_exp T_Const(int);
T_exp T_Call(T_exp, T_expList);

T_relOp T_notRel(T_relOp);  /* a op b    ==     not(a notRel(op) b)  */
T_relOp T_commute(T_relOp); /* a op b    ==    b commute(op) a       */

#endif /* TIGER_TREE_H_ */
