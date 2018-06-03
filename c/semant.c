#include "semant.h"
#include "absync.h"
#include "types.h"
#include "translate.h"
#include "env.h"
#include "errormsg.h"
#include <assert.h>

struct expty Expty(Tr_exp exp, Ty_ty ty);
static Ty_ty transSUType(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_type type);
static Ty_spec transSpec(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_spec spec);
static Ty_dec transDec(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_dec dec);

struct expty Expty(Tr_exp exp, Ty_ty ty)
{
    struct expty e;
    e.exp = exp; e.ty = ty;
    return e;
}

//add variable to struct or union, calc size
static void _transSDeclar(Tr_level level, E_linkage linkenv, E_namespace nameenv, Ty_ty sty, A_declaration dec)
{
    switch(dec->kind)
    {
        case A_simple_declaration:
        {
            Ty_spec spec = transSpec(level, linkenv, nameenv, dec->u.simple.spec);
            Ty_dec   dec_ = transDec(level, linkenv, nameenv, dec->u.simple.dec);
            Ty_dec temp = dec_;
            while(temp)
            {
                //todo
                Ty_dec declar = Ty_specdec(spec,dec)
                temp = temp->next;
            }
            break;
        }
        case A_seq_declaration:
            _transSDeclar(level, linkenv, nameenv, sty, dec->u.seq.declaration);
            _transSDeclar(level, linkenv, nameenv, sty, dec->u.seq.next);
            break;
        default:
            assert(0);
    }
}

//translate Struct or Union type
static Ty_ty transSUType(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_type type)
{
    Ty_ty resty = checked_malloc(sizeof(*resty));
    int isStruct = 0;

    assert(type->kind == A_struct_union_type);
    isStruct = type->u.struct_union.struct_union == A_STRUCT;
    resty->kind = isStruct ? Ty_structTy : Ty_unionTy;
    resty->sign = 0;
    resty->align = SIZE_ALIGN;
    resty->size = 0;
    resty->incomplete = 1;

    A_declaration dec_list = type->u.struct_union.dec_list;
    if (dec_list != NULL)
    {
        if (type->u.struct_union.id)
            S_enter(nameenv->tenv, type->u.struct_union.id, resty);
        nameenv->tenv = S_beginScope(S_BLOCK, nameenv->tenv);
        nameenv->venv = S_beginScope(S_BLOCK, nameenv->venv);
        _transSDeclar(level, linkenv, nameenv, resty, dec_list);
        nameenv->tenv = S_endScope(nameenv->tenv);
        nameenv->venv = S_endScope(nameenv->venv);
    }

    return resty;
}

//trans type spec
static void _transType(Tr_level level, E_linkage linkenv, E_namespace nameenv, Ty_spec ty_spec, A_type type)
{
    switch(type->kind)
    {
        case A_simple_type:
            if (ty_spec->complex_type)
                EM_error(type->pos, "more than one type assigned");
            else
            {
                switch(type->u.simple)
                {
                    case A_VOID:
                        if (Ty_isCHAR(ty_spec) || Ty_isSHORT(ty_spec) || Ty_isINT(ty_spec)
                            ||Ty_isLONG(ty_spec) || Ty_isFLOAT(ty_spec) || Ty_isDOUBLE(ty_spec)
                            ||Ty_isSIGNED(ty_spec) || Ty_isUNSIGNED(ty_spec))
                            EM_error(type->pos, "void type can not be along with other type");
                        else
                            ty_spec->specs |= Ty_VOID;
                        break;
                    case A_CHAR:
                        if (Ty_isSHORT(ty_spec) || Ty_isINT(ty_spec) ||Ty_isLONG(ty_spec) 
                         || Ty_isFLOAT(ty_spec) || Ty_isDOUBLE(ty_spec))
                            EM_error(type->pos, "char type can not be along with this type");
                        else if (Ty_isCHAR(ty_spec))
                            EM_error(type->pos, "char type can only appear once");
                        else
                            ty_spec->specs |= Ty_CHAR;
                        break;
                    case A_SHORT:
                        if (Ty_isVOID(ty_spec) || Ty_isCHAR(ty_spec) || Ty_isLONG(ty_spec) 
                         || Ty_isFLOAT(ty_spec) || Ty_isDOUBLE(ty_spec))
                            EM_error(type->pos, "short type can not be along with this type"); 
                        else if (Ty_isSHORT(ty_spec))
                            EM_error(type->pos, "short type can only appear once");
                        else
                            ty_spec->specs |= Ty_SHORT;
                        break;
                    case A_INT:
                        if (Ty_isVOID(ty_spec) || Ty_isCHAR(ty_spec) || Ty_isFLOAT(ty_spec) 
                         || Ty_isDOUBLE(ty_spec))
                            EM_error(type->pos, "int type can not be along with this type");
                        else if (Ty_isINT(ty_spec))
                            EM_error(type->pos, "int type can only appear once");
                        else
                            ty_spec->specs |= Ty_INT;
                        break;
                    case A_LONG:
                        if (Ty_isVOID(ty_spec) || Ty_isCHAR(ty_spec) || Ty_isINT(ty_spec)
                            || Ty_isFLOAT(ty_spec))
                            EM_error(type->pos, "long type can not be along with other type");
                        else if (ty_spec->LONG == 2)
                            EM_error(type->pos, "long type can only appear twice");
                        else
                            ty_spec->LONG += 1;
                        break;
                    case A_FLOAT:
                        if (Ty_isVOID(ty_spec) || Ty_isCHAR(ty_spec) || Ty_isSHORT(ty_spec) 
                         || Ty_isINT(ty_spec) ||Ty_isLONG(ty_spec) || Ty_isDOUBLE(ty_spec)
                            ||Ty_isSIGNED(ty_spec) || Ty_isUNSIGNED(ty_spec))
                            EM_error(type->pos, "float type can not be along with other type");
                        else if (Ty_isFLOAT(ty_spec))
                            EM_error(type->pos, "float type can only appear once");
                        else
                            ty_spec->specs |= Ty_FLOAT;
                        break;
                    case A_DOUBLE:
                        if (Ty_isVOID(ty_spec) || Ty_isCHAR(ty_spec) || Ty_isSHORT(ty_spec) 
                         || Ty_isINT(ty_spec) || Ty_isFLOAT(ty_spec)
                            ||Ty_isSIGNED(ty_spec) || Ty_isUNSIGNED(ty_spec))
                            EM_error(type->pos, "double type can not be along with this type");
                        else if (Ty_isDOUBLE(ty_spec))
                            EM_error(type->pos, "double type can only appear once");
                        else
                            ty_spec->specs |= Ty_DOUBLE;
                        break;
                    case A_SIGNED:
                        if (Ty_isVOID(ty_spec) || Ty_isFLOAT(ty_spec) || Ty_isDOUBLE(ty_spec)
                            || Ty_isUNSIGNED(ty_spec))
                            EM_error(type->pos, "signed type can not be along with this type");
                        else if (Ty_isSIGNED(ty_spec))
                            EM_error(type->pos, "signed type can only appear once");
                        else
                            ty_spec->specs |= Ty_SIGNED;
                        break;
                    case A_UNSIGNED:
                        if (Ty_isVOID(ty_spec) || Ty_isFLOAT(ty_spec) || Ty_isDOUBLE(ty_spec)
                            || Ty_isSIGNED(ty_spec))
                            EM_error(type->pos, "unsigned type can not be along with this type");
                        else if (Ty_isUNSIGNED(ty_spec))
                            EM_error(type->pos, "unsigned type can only appear once");
                        else
                            ty_spec->specs |= Ty_UNSIGNED;
                        break;
                    default:
                        assert(0);
                }
            }
            break;
        case A_struct_union_type:
            if (Ty_isSimpleType(ty_spec) || ty_spec->complex_type)
                EM_error(type->pos, "more than one type assigned");
            else 
                ty_spec->complex_type = transSUType(level, linkenv, nameenv, type);
            break;
        case A_enumtype_type:
            if (Ty_isSimpleType(ty_spec) || ty_spec->complex_type)
                EM_error(type->pos, "more than one type assigned");
            else 
                ty_spec->complex_type = transEType(type);
            break;
        case A_typeid_type:
            if (Ty_isSimpleType(ty_spec) || ty_spec->complex_type)
                EM_error(type->pos, "more than one type assigned");
            else 
            {
                Ty_ty t = S_look(nameenv->venv, type->u.typeid);
                if (t == NULL || t->kind != Ty_name)
                    EM_error(type->pos, "no such type");
                else
                    ty_spec->complex_type = t->u.name;
            }
            break;
        default:
            assert(0);
    }
}

#define TRANSSPEC(NAME) \
case A_##NAME : \
	if (Ty_is##NAME(ty_spec)) \
		EM_error(spec->pos, "#NAME" "can only appear once"); \
	else \
		ty_spec->specs |= Ty_##NAME; \
	break; 
static void _transSpec(Tr_level level, E_linkage linkenv, E_namespace nameenv, Ty_spec ty_spec, A_spec spec)
{
	switch(spec->kind)
	{
		case A_storage_spec:
			switch(spec->u.storage)
			{
				TRANSSPEC(TYPEDEF);
				TRANSSPEC(EXTERN);
				TRANSSPEC(STATIC);
				TRANSSPEC(AUTO);
				TRANSSPEC(REGISTER);
				default:
					assert(0);
			}
			break;
		case A_qual_type_spec:
			switch(spec->u.qual_type)
			{
				TRANSSPEC(CONST);
				TRANSSPEC(RESTRICT);
				TRANSSPEC(VOLATILE);
				default:
					assert(0);
			}
			break;
		case A_func_type_spec:
			switch(spec->u.func_type)
			{
				TRANSSPEC(INLINE);
				default:
					assert(0);
			}
			break;
		case A_seq_spec:
			_transSpec(level, linkenv, nameenv, ty_spec, spec->u.seq.spec);
            if (spec->u.seq.next)
			    _transSpec(level, linkenv, nameenv, ty_spec, spec->u.seq.next);
			break;
		case A_type_spec:
            _transType(level, linkenv, nameenv, ty_spec, spec->u.type);
			break;
		default:
			assert(0);
	}
}

//trans spec
static Ty_spec transSpec(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_spec spec)
{
    Ty_spec p = checked_malloc(sizeof(*p));
    _transSpec(level, linkenv, nameenv, p, spec);
    return p;
}
#undef TRANSSPEC




//trans for pointer type
static void _transPointer(Tr_level level, E_linkage linkenv, E_namespace nameenv, Ty_dec ty_dec, A_pointer pointer)
{
    switch(pointer->kind)
    {
        case A_simple_pointer:
        {
            Ty_spec spec = transSpec(level, linkenv, nameenv, pointer->u.simple);
            ty_dec->type = Ty_PointerTy(ty_dec->type, spec->specs);
            free(spec);
            break;
        }
        case A_seq_pointer:
            _transPointer(level, linkenv, nameenv, ty_dec, pointer->u.seq.pointer);
            _transPointer(level, linkenv, nameenv, ty_dec, pointer->u.seq.next);
            break;
        default:
            assert(0);
    }
}

//put the dec in ty_dec and return
static void _transDec(Tr_level level, E_linkage linkenv, E_namespace nameenv, Ty_dec ty_dec, A_dec dec)
{
    switch(dec->kind)
    {
        case A_simple_dec:
            ty_dec->sym = dec->u.simple;
            break;
        case A_seq_dec:
        {
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.seq.dec);
            Ty_dec tail = transDec(level, linkenv, nameenv, dec->u.seq.next);
            ty_dec = Ty_DecList(ty_dec, tail);
            break;
        }
        case A_init_dec:
            ty_dec->init = dec->u.init.init;
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.init.dec);
            break;
        case A_bit_dec://todo check
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.bit.dec);
            if (ty_dec->type)
                EM_error(dec->pos, "invalid bitfield type");
            else
                ty_dec->type = Ty_BitTy(NULL, dec->u.bit.const_exp);
            break;
        case A_pointer_dec:
            _transPointer(level, linkenv, nameenv, ty_dec, dec->u.pointer.pointer);
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.pointer.dec);
            break;
        case A_array_dec:
            ty_dec->type = Ty_ArrayTy(ty_dec->type, dec->u.array.assign_exp);
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.array.dec);
            break;
        case A_func_dec:
        {
            Ty_fieldList params = transParam(level, linkenv, nameenv, dec->u.func.param_list);//todo
            ty_dec->type = Ty_FuncTy(ty_dec->type, params);
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.func.dec);
            break;
        }
        default:
            assert(0);
    }
}

//trans declator
static Ty_dec transDec(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_dec dec)
{
    Ty_dec p = checked_malloc(sizeof(*p));
    _transDec(level, linkenv, nameenv, p, dec);
    return p;
}

/*
struct expty transExp(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_exp a){
    switch (a->kind){
        case A_id_exp :
            E_enventry e = S_look(nameenv->venv, a->u.id);
            if (e == NULL || e->kind != E_varEntry)
                EM_error(a->pos, "No such variable");
            return Expty(Tr_simpleVar(level, e->u.access), e->u.var.ty);//access中需要有size啥的
        case A_intexp_exp :
            return Expty(Tr_intExp(a->u.intexp), Ty_INT_());
        case A_floatexp_exp :
            return Expty(Tr_floatExp(a->u.floatexp), Ty_Float());
        case A_charexp_exp :
            return Expty(Tr_charExp(a->u.charexp), Ty_Char());
        case A_strexp_exp :
            return Expty(Tr_strExp(a->u.strexp), Ty_ConstP(Ty_Char()));
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
*/
