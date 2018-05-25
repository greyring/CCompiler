#include "temp.h"
#include "tree.h"

T_stm _T_seq_stm(T_stm left, T_stm right)
{
	T_stm p = checked_malloc(sizeof(*p));
	p->kind = T_seq_stm;
	p->u.seq.left = left;
	p->u.seq.right = right;
}

T_stm _T_label_stm(Temp_label label)
{
	T_stm p = checked_malloc(sizeof(*p));
	p->kind = T_label_stm;
	p->u.label = label;
}


