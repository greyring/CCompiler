#ifndef _TESTAST_H_
#define _TESTAST_H_

#include "absync.h"
#include "symbol.h"
char* TA_getRes();

void TA_symbol(S_symbol in);
void TA_exp(A_exp in);
void TA_spec(A_spec in);
void TA_type(A_type in);
void TA_pointer(A_pointer in);
void TA_param(A_param in);
void TA_dec(A_dec in);
void TA_declaration(A_declaration in);
void TA_type_name(A_type_name in);
void TA_designator(A_designator in);
void TA_init(A_init in);
void TA_stat(A_stat in);
void TA_def(A_def in);
void TA_op(A_op in);
void TA_storage_type(A_storage_type in);
void TA_prim_type(A_prim_type in);
void TA_qual_type(A_qual_type in);
void TA_func_type(A_func_type in);

#endif