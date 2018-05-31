#ifndef SEMANT_H
#define SEMANT_H

struct expty {Tr_exp exp; Ty_ty ty;};

struct expty Expty(Tr_exp exp, Ty_ty ty);

void printError(char* c);

struct expty transExp(S_table venv, S_table tenv, A_exp a);
struct expty transSpec(S_table venv, S_table tenv, A_spec a);
struct expty transType(S_table venv, S_table, tenv, A_type a);
struct expty transPointer(S_table venv, S_table tenv, A_pointer a);
struct expty transParam(S_table venv, S_table tenv, A_param a);
struct expty transDec(S_table venv, S_table tenv, A_dec a);
struct expty transDeclaration(S_table venv, S_table tenv, A_declaration a);
struct expty transTypeName(S_table venv, S_table tenv, A_type_name a);
struct expty transDesignator(S_table venv, S_table tenv, A_designator a);
struct expty transInit(S_table venv, S_table tenv, A_init a);
struct expty transStat(S_table venv, S_table tenv, A_stat a);
struct expty transDef(S_table venv, S_table tenv, A_def a);

#endif