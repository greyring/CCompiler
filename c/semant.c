#include "translate.h"
#include "semant.h"
#include "absync.h"
#include "table.h"
#include "symbol.h"
#include "util.h"
#include "types.h"
#include "env.h"
#include <string.h>

struct expty Expty(Tr_exp exp, Ty_ty ty){
    struct expty e;
    e.exp = exp; e.ty = ty;
    return e;
}

void printError(int line, char* c){
    printf("Line %d : %s\n", line, c);
    exit(0);
}

/*
Only basic type
No pointer
Only int and float
*/

struct expty transExp(S_table venv, S_table tenv, A_exp a){
    switch (a->kind){
        case A_id_exp : {
            E_enventry e = S_look(venv, a->u.id);
            if (e == NULL){
                printError(a->pos, "No such variable");
            }
            return Expty(NULL, e->u.var.ty);
            break;
        }
        case A_intexp_exp : {
            return Expty(NULL, Ty_Int());
            break;
        }
        case A_floatexp_exp : {
            return Expty(NULL, Ty_Float());
            break;
        }
        case A_charexp_exp : {
            return Expty(NULL, Ty_Char());
            break;
        }
        case A_strexp_exp : {
            return Expty(NULL, Ty_Array(Ty_Char(), strlen(a->u.strexp)));
            break;
        }
        case A_subscript_exp : {
            Expty idExp = transExp(venv, tenv, a->u.subscript.expr);
            Expty sbExp = transExp(venv, tenv, a->u.subscript.subscript);
            if (sbExp.ty->kind != Ty_int){
                printError(a->pos, "Subscript needs to be int type");
            }
            if (idExp.ty->kind != Ty_array){
                printError(a->pos, "id needs to be array type");
            }
            return Expty(NULL, idExp.ty->u.array.array);
            break;
        }
        case A_funccall_exp : {//Simple One
            if (a->u.funccal.expr == NULL || a->u.funccall.expr->kind != A_id_exp){
                printError(a->pos, "func needs a name");
            }
            Symbol sym = a->u.funccall.expr->u.id;
            E_enventry e = S_look(tenv, id);

            if (e == NULL){
                printError(a->pos, "func not defined");
            }

            if (e->kind != E_funEntry){
                printError(a->pos, "not a func");
            }

            Ty_tyList formals = e->u.fun.formals;
            A_exp args = a->u.funccall.args;

            if (args == NULL){
                if (formals != NULL){
                    printError(a->pos, "func need params");
                }
                else{
                    return Expty(NULL, e->u.fun.result);
                }
            }
            else while (1){
                Expty exp;
                if (args->kind == A_seq_exp){
                    exp = transExp(venv, tenv, args->u.expr);
                    if (formals->head == NULL || formals->head->kind != exp.ty->kind){
                        printError(a->pos, "Unmatched params");
                    }
                    formals = formals->tail;
                    args = args->u.seq.next;
                }
                else{
                    exp = transExp(venv, tenv, args);
                    if (formals->head == NULL || formals->head->kind != exp.ty->kind){
                        printError(a->pos, "Unmatched params");
                    }
                    formals = formals->tail;
                    if (formals != NULL){
                        printError(a->pos, "Umatched params");
                    }
                    break;
                }
            }

            return Expty(NULL, e->u.fun.result);
            break;
        }
        case A_dot_exp : {//struct expr.id
            break;
        }
        case A_point_exp : {//struct expr->id
            break;
        }
        case A_postpp_exp : {
            Expty exp = transExp(venv, tenv, a->u.postpp);
            if (exp.ty->kind != Ty_int){
                printError(a->pos, "Only integer can operate plus plus");
            }
            return Expty(NULL, Ty_Int);
            break;
        }
        case A_postmm_exp : {
            Expty exp = transExp(venv, tenv, a->u.postpp);
            if (exp.ty->kind != Ty_int){
                printError(a->pos, "Only integer can operate minus minus");
            }
            return Expty(NULL, Ty_Int);
            break;
        }
        case A_init_exp : {
            break;
        }
        
        case A_prepp_exp : {
            Expty exp = transExp(venv, tenv, a->u.postpp);
            if (exp.ty->kind != Ty_int){
                printError(a->pos, "Only integer can operate plus plus");
            }
            return Expty(NULL, Ty_Int);
            break;
        }
        case A_premm_exp : {
            Expty exp = transExp(venv, tenv, a->u.postpp);
            if (exp.ty->kind != Ty_int){
                printError(a->pos, "Only integer can operate minus minus");
            }
            return Expty(NULL, Ty_Int);
            break;
        }
        case A_unaryop_exp : {
            Expty exp = transExp(venv, tenv, a->u.unaryop.expr);
            if ((exp.ty->kind != Ty_int   || 
                 exp.ty=>kind != Ty_float)&&
                (op == A_PLUS || op == A_MINUS)){
                    printError(a->pos, "Unmatched operation");
            }
            if (exp.ty->kind != Ty_ind && op == A_NOT){
                printError(a->pos, "Unmatched operation");
            }

            return Expty(NULL, exp.ty);
            break;
        }
        case A_sizeof_unary_exp : {
            break;
        }
        case A_sizeof_type_exp : {
            break;
        }

        case A_cast_exp : {//TODO
            Expty exp = transExp(vnev, tenv, a->u.cast.expr);
            return Expty();
            break;
        }

        case A_binop_exp : {
            Expty left = transExp(venv, tenv, a->u.binop.expr1);
            Expty right= transExp(venv, tenv, a->u.binop.expr2);
            Expty ret;
            switch (a->u.binop.op){//only the same type
                case A_PLUS : {
                    if (left.ty->kind != right.ty->kind){
                        printError(a->pos, "Left and right type should be the same");
                    }
                    if (left.ty->kind != Ty_int || left.ty->kind != Ty_float){
                        printError(a->pos, "Only int or float can operate plus");
                    }
                    return Expty(NULL, left.ty);
                }
                case A_MINUS : {
                    if (left.ty->kind != right.ty->kind){
                        printError(a->pos, "Left and right type should be the same");
                    }
                    if (left.ty->kind != Ty_int || left.ty->kind != Ty_float){
                        printError(a->pos, "Only int or float can operate minus");
                    }
                    return Expty(NULL, left.ty);
                }
                case A_MUL : {
                    if (left.ty->kind != right.ty->kind){
                        printError(a->pos, "Left and right type should be the same");
                    }
                    if (left.ty->kind != Ty_int || left.ty->kind != Ty_float){
                        printError(a->pos, "Only int or float can operate mul");
                    }
                    return Expty(NULL, left.ty);
                }
                case A_DIV : {
                    if (left.ty->kind != right.ty->kind){
                        printError(a->pos, "Left and right type should be the same");
                    }
                    if (left.ty->kind != Ty_int || left.ty->kind != Ty_float){
                        printError(a->pos, "Only int or float can operate div");
                    }
                    return Expty(NULL, left.ty);
                }
                case A_MOD : {
                    if (left.ty->kind != right.ty->kind){
                        printError(a->pos, "Left and right type should be the same");
                    }
                    if (left.ty->kind != Ty_int){
                        printError(a->pos, "Only int can operate mod");
                    }
                    return Expty(NULL, left.ty);
                }
                case A_AND : {
                    if (left.ty->kind != right.ty->kind){
                        printError(a->pos, "Left and right type should be the same");
                    }
                    if (left.ty->kind != Ty_int){
                        printError(a->pos, "Only int can operate and");
                    }
                    return Expty(NULL, left.ty);
                }
                case A_XOR : {
                    if (left.ty->kind != right.ty->kind){
                        printError(a->pos, "Left and right type should be the same");
                    }
                    if (left.ty->kind != Ty_int){
                        printError(a->pos, "Only int can operate xor");
                    }
                    return Expty(NULL, left.ty);
                }
                case A_OR : {
                    if (left.ty->kind != right.ty->kind){
                        printError(a->pos, "Left and right type should be the same");
                    }
                    if (left.ty->kind != Ty_int){
                        printError(a->pos, "Only int can operate or");
                    }
                    return Expty(NULL, left.ty);
                }
                case A_LOGAND : {
                    
                }
            }
            break;
        }
        case A_threeop_exp : {
            break;
        }

        case A_assign_exp : {
            break;
        }
        case A_seq_exp : {
            transExp(a->u.seq.expr);
            return transExp(a->u.seq.next);
            break;
        }

    }
}

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

