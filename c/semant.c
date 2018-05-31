#include "env.h"
#include "translate.h"
#include "semant.h"
#include "absync.h"
#include "errormsg.h"


//建立一个expty
struct expty_ 
expTy(Tr_exp exp, Ty_ty ty)
{
    struct expty_ e;
    e.exp = exp; 
    e.ty = ty;
    return e;
}


//A_exp语义分析
struct expty_ 
transExp(S_table venv/*值环境*/, S_table tenv/*类型环境*/, A_exp a)
{
    switch (a->kind)
    {
        //检查变量，下标和域, =transVar()
        case A_id_exp:{
            //检查变量是不是varEntry类型，不是的话报错
            E_enventry x = (E_enventry)S_look(venv, a->u.id);
            if(x && x->kind==E_varEntry){
                //类型检查无误
                return expTy(NULL, x->u.var.ty);//这里没用actual_ty(x->u.var.ty)函数，不知道是否要用它
            }
            else{
                //类型检查出错
                EM_error(a->pos, "undefined variable %s", S_name(a->u.id));
                return expTy(NULL, Ty_INT());
            }
            break;
        }
        // return expTy(Tr_access(a->u.id), ?);//todo


        case A_intexp_exp:{
            E_enventry x = (E_enventry)S_look(tenv, a->u.intexp.id);
            if(x && x->kind==E_varEntry && x->u.var.ty==Ty_constant){

            }
            return expty_prim(a->u.intexp);//todo
            break;
        }
            
        case A_floatexp_exp:
            return expty_prim(a->u.floatexp);//rodo
            break;

        case A_charexp_exp:
            return expty_prim(a->u.charexp);//todo
            break;

        case A_strexp_exp:
            break;
            //todo

        
        case A_subscript_exp:
            struct expty_ expr, subscript;
            expr = transExp(venv, tenv, a->u.subscript.expr);//pointer type or array type
            subscript = transExp(venv, tenv, a->u.subscript.subscript);//int type
            assert(expr.ty is integer); // is not id?
            assert(subscript is pointer); // is not integer?
            return expTy(Tr_subscript_exp(expr, subscript), ?);//todo
        
        
        case A_funccall_exp:
            //todo
        
        
        case A_dot_exp:


        case A_point_exp:

        case A_postpp_exp://这是什么

        case A_postmm_exp://这是什么


        case A_init_exp:


        
        
        /*
        A_dot_exp,
        A_point_exp,
        A_postpp_exp,
        A_postmm_exp,
        A_init_exp,
        //unary
        A_prepp_exp,
        A_premm_exp,
        A_unaryop_exp,
        A_sizeof_unary_exp,
        A_sizeof_type_exp,
        //cast
        A_cast_exp,
        //op
        A_binop_exp,
        A_threeop_exp,
        //assign
        A_assign_exp,
        A_seq_exp*/
    }
}

//A_spec语义分析
// specifier有点复杂，暂时忽略
int 
noStoreType(A_spec a, A_storage_type type)
{
    int res = 0;
    if (!a) return 1;
    switch(a->kind)
    {
        case A_seq_spec:
            res = noStoreType(a->u.seq.spec, type) && noStoreType(a->u.seq.next, type);
            break;
        case A_storage_spec:
            res = a->u.storage == type;
            break;
        case A_type_spec:case A_qual_type_spec:case A_func_type_spec: 
            res = 1;
            break;
    }
    return res;
}


//frame模块定义F_fragList
//A_def类型检查
F_fragList 
transDef(A_def a)
{
    if (!a) return NULL;
    switch(a->kind)
    {
        case A_seq_def:
            return F_FragList(transDef(a->u.seq.next), transDef(a->u.seq.def));//reverse order
        case A_simple_def:
            //assert(a->u.simple->kind == A_simple_declaration);
            //assert(noStoreType(a->u.simple->u.simple.spec, A_AUTO));//todo
            //assert(noStoreType(a->u.simple->u.simple.spec, A_REGISTER));//todo
            //declarationNameOnce(a->u.simple);

            break;
        case A_func_def:
            //assert(noStoreType(a->u.func.spec, A_AUTO));//todo
            //assert(noStoreType(a->u.func.spec, A_REGISTER));//todo

            break;
    }
}


//语义分析主接口，遍历AST
F_fragList 
SEM_transProg(A_def root)
{
    E_envs envs = E_base_envs();
    return transDef(Tr_outermost(), envs, root);
}