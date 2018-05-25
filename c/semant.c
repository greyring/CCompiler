#include "env.h"
#include "translate.h"
#include "semant.h"

struct expty trexp2expty(Tr_exp exp, Ty_ty ty)
{
    struct expty e;
    e.exp = exp; e.ty = ty;
    return e;
}

static struct expty transExp(A_exp a)
{
    switch (a->kind)
    {
        case A_id_exp:

            return trexp2expty(Tr_access(a->u.id), ?);//todo
        case A_intexp_exp:
            return expty_prim(a->u.intexp);//todo
        case A_floatexp_exp:
            return expty_prim(a->u.floatexp);//rodo
        case A_charexp_exp:
            return expty_prim(a->u.charexp);//todo
        case A_strexp_exp:
            //todo
        case A_subscript_exp:
            struct expty expr, subscript;
            expr = transExp(a->u.subscript.expr);//pointer type or array type
            subscript = transExp(a->u.subscript.subscript);//int type
            assert(expr.ty is integer);
            assert(subscript is pointer);
            return trexp2expty(Tr_subscript_exp(expr, subscript), ?);//todo
        case A_funccall_exp:
            //todo
        case A_dot_exp:

        
        
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

static int noStoreType(A_spec a, A_storage_type type)
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

F_fragList transDef(A_def a)
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

F_fragList SEM_transProg(A_def root)
{
    E_envs envs = E_base_envs();
    return transDef(Tr_outermost(), envs, root);
}