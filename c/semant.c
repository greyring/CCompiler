#include "semant.h"
#include "absync.h"
#include "types.h"
#include "translate.h"
#include "env.h"
#include "errormsg.h"
#include <assert.h>

static struct expty transExp(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_exp);
static Ty_ty transEType(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_type type);
static Ty_ty transSUType(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_type type);
static Ty_spec transSpec(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_spec spec);
static Ty_decList transDec(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_dec dec);
static Tr_exp transInit(Tr_level level, E_linkage linkenv, E_namespace nameenv, E_enventry entry, A_init init);
static Tr_exp transDeclar(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_declaration declar);
static Tr_exp transStat(Tr_level level, E_linkage linkenv, E_namespace nameenv, Temp_label contl, Temp_label breakl, A_stat stat);
static Tr_exp transDef(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_def def);

static struct expty Expty(Tr_exp exp, Ty_ty ty)
{
    struct expty e;
    e.exp = exp; e.ty = ty;
    return e;
}

static exptyNode SE_ExptyNode(struct expty expty)
{
    exptyNode p = checked_malloc(sizeof(*p));
    p->expty = expty;
    p->next = NULL;
    return p;
}

static exptyList SE_ExptyList(exptyList exptyList, exptyNode exptyNode)
{
    if (exptyList.head == NULL)
    {
        exptyList.head = exptyList.tail = NULL;
    }
    exptyList.tail = exptyList.tail->next = exptyNode;
    return exptyList;
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
                        if (Ty_isCHAR(ty_spec->specs) || Ty_isSHORT(ty_spec->specs) || Ty_isINT(ty_spec->specs)
                            ||Ty_isLONG(ty_spec) || Ty_isFLOAT(ty_spec->specs) || Ty_isDOUBLE(ty_spec->specs)
                            ||Ty_isSIGNED(ty_spec->specs) || Ty_isUNSIGNED(ty_spec->specs))
                            EM_error(type->pos, "void type can not be along with other type");
                        else
                            ty_spec->specs |= Ty_VOID;
                        break;
                    case A_CHAR:
                        if (Ty_isSHORT(ty_spec->specs) || Ty_isINT(ty_spec->specs) ||Ty_isLONG(ty_spec) 
                         || Ty_isFLOAT(ty_spec->specs) || Ty_isDOUBLE(ty_spec->specs))
                            EM_error(type->pos, "char type can not be along with this type");
                        else if (Ty_isCHAR(ty_spec->specs))
                            EM_error(type->pos, "char type can only appear once");
                        else
                            ty_spec->specs |= Ty_CHAR;
                        break;
                    case A_SHORT:
                        if (Ty_isVOID(ty_spec->specs) || Ty_isCHAR(ty_spec->specs) || Ty_isLONG(ty_spec) 
                         || Ty_isFLOAT(ty_spec->specs) || Ty_isDOUBLE(ty_spec->specs))
                            EM_error(type->pos, "short type can not be along with this type"); 
                        else if (Ty_isSHORT(ty_spec->specs))
                            EM_error(type->pos, "short type can only appear once");
                        else
                            ty_spec->specs |= Ty_SHORT;
                        break;
                    case A_INT:
                        if (Ty_isVOID(ty_spec->specs) || Ty_isCHAR(ty_spec->specs) || Ty_isFLOAT(ty_spec->specs) 
                         || Ty_isDOUBLE(ty_spec->specs))
                            EM_error(type->pos, "int type can not be along with this type");
                        else if (Ty_isINT(ty_spec->specs))
                            EM_error(type->pos, "int type can only appear once");
                        else
                            ty_spec->specs |= Ty_INT;
                        break;
                    case A_LONG:
                        if (Ty_isVOID(ty_spec->specs) || Ty_isCHAR(ty_spec->specs) || Ty_isINT(ty_spec->specs)
                            || Ty_isFLOAT(ty_spec->specs))
                            EM_error(type->pos, "long type can not be along with other type");
                        else if (ty_spec->LONG == 2)
                            EM_error(type->pos, "long type can only appear twice");
                        else
                            ty_spec->LONG += 1;
                        break;
                    case A_FLOAT:
                        if (Ty_isVOID(ty_spec->specs) || Ty_isCHAR(ty_spec->specs) || Ty_isSHORT(ty_spec->specs) 
                         || Ty_isINT(ty_spec->specs) ||Ty_isLONG(ty_spec) || Ty_isDOUBLE(ty_spec->specs)
                            ||Ty_isSIGNED(ty_spec->specs) || Ty_isUNSIGNED(ty_spec->specs))
                            EM_error(type->pos, "float type can not be along with other type");
                        else if (Ty_isFLOAT(ty_spec->specs))
                            EM_error(type->pos, "float type can only appear once");
                        else
                            ty_spec->specs |= Ty_FLOAT;
                        break;
                    case A_DOUBLE:
                        if (Ty_isVOID(ty_spec->specs) || Ty_isCHAR(ty_spec->specs) || Ty_isSHORT(ty_spec->specs) 
                         || Ty_isINT(ty_spec->specs) || Ty_isFLOAT(ty_spec->specs)
                            ||Ty_isSIGNED(ty_spec->specs) || Ty_isUNSIGNED(ty_spec->specs))
                            EM_error(type->pos, "double type can not be along with this type");
                        else if (Ty_isDOUBLE(ty_spec->specs))
                            EM_error(type->pos, "double type can only appear once");
                        else
                            ty_spec->specs |= Ty_DOUBLE;
                        break;
                    case A_SIGNED:
                        if (Ty_isVOID(ty_spec->specs) || Ty_isFLOAT(ty_spec->specs) || Ty_isDOUBLE(ty_spec->specs)
                            || Ty_isUNSIGNED(ty_spec->specs))
                            EM_error(type->pos, "signed type can not be along with this type");
                        else if (Ty_isSIGNED(ty_spec->specs))
                            EM_error(type->pos, "signed type can only appear once");
                        else
                            ty_spec->specs |= Ty_SIGNED;
                        break;
                    case A_UNSIGNED:
                        if (Ty_isVOID(ty_spec->specs) || Ty_isFLOAT(ty_spec->specs) || Ty_isDOUBLE(ty_spec->specs)
                            || Ty_isSIGNED(ty_spec->specs))
                            EM_error(type->pos, "unsigned type can not be along with this type");
                        else if (Ty_isUNSIGNED(ty_spec->specs))
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
                ty_spec->complex_type = transEType(level, linkenv, nameenv, type);
            break;
        case A_typeid_type:
            if (Ty_isSimpleType(ty_spec) || ty_spec->complex_type)
                EM_error(type->pos, "more than one type assigned");
            else 
            {
                Ty_ty t = S_look(nameenv->venv, type->u.typeid);
                if (t == NULL || t->kind != Ty_nameTy)
                    EM_error(type->pos, "no such type");
                else
                    ty_spec->complex_type = t->u.nameTy;
            }
            break;
        default:
            assert(0);
    }
}

#define TRANSSPEC(NAME) \
case A_##NAME : \
	if (Ty_is##NAME(ty_spec->specs)) \
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

static Ty_field _transParam(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_param param)
{
    Ty_field field = NULL;
    switch(param->kind)
    {
        case A_dec_param:
        {
            Ty_spec spec = transSpec(level, linkenv, nameenv, param->u.dec.spec);
            Ty_decList  decList = transDec(level, linkenv, nameenv, param->u.dec.dec);
            Ty_dec dec = decList.head;
            assert(dec->next == NULL);//must be single
            dec = Ty_specdec(spec, dec);
            field = Ty_Field(dec->type, dec->sym);
            break;
        }
        case A_uncertain_param:
            field = Ty_Field(NULL, NULL);//is uncertain param
            break;
        default:
            assert(0);
    }
    return field;
}

static Ty_fieldList transParam(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_param param)
{
    Ty_fieldList fieldList;
    Ty_field tail = NULL;
    fieldList.head = fieldList.tail = NULL;
    if (param == NULL)
        return fieldList;
    tail = checked_malloc(sizeof(*tail));
    switch(param->kind)
    {
        case A_seq_param:
            fieldList = transParam(level, linkenv, nameenv, param->u.seq.param);
            tail = _transParam(level, linkenv, nameenv, param->u.seq.next);
            fieldList = Ty_FieldList(fieldList, tail);
            break;
        default:
            tail = _transParam(level, linkenv, nameenv, param);
            fieldList = Ty_FieldList(fieldList, tail);
            break;
    }
    return fieldList;
}



//dec: not a seq dec 
//return: single dec
static Ty_dec _transDec(Tr_level level, E_linkage linkenv, E_namespace nameenv, Ty_dec ty_dec, A_dec dec)
{
    switch(dec->kind)
    {
        case A_simple_dec:
            ty_dec->sym = dec->u.simple;
            break;
        case A_init_dec:
            ty_dec->init = dec->u.init.init;
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.init.dec);
            break;
        case A_bit_dec://todo check
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.bit.dec);
            if (ty_dec->type)
                EM_error(dec->pos, "invalid bitfield type");
            else
            {
                struct expty expty = transExp(level, linkenv, nameenv, dec->u.bit.const_exp);
                if (Ty_isIntTy(expty.ty) && Ty_isBasicCTy(expty.ty))
                    ty_dec->type = Ty_BitTy(NULL, Tr_getIntConst(expty.exp));
                else
                    EM_error(dec->pos, "bit field should be int constant");
            }
            break;
        case A_pointer_dec:
            _transPointer(level, linkenv, nameenv, ty_dec, dec->u.pointer.pointer);
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.pointer.dec);
            break;
        case A_array_dec:
        {
            struct expty expty = transExp(level, linkenv, nameenv, dec->u.array.assign_exp);
            ty_dec->type = Ty_ArrayTy(ty_dec->type, expty.exp, expty.ty);
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.array.dec);
            break;
        }
        case A_func_dec:
        {
            //if ()//todo
            E_BeginScope(S_FUNCPRO, nameenv);
            Ty_fieldList params = transParam(level, linkenv, nameenv, dec->u.func.param_list);
            E_EndScope(nameenv);
            ty_dec->type = Ty_FuncTy(ty_dec->type, params);
            _transDec(level, linkenv, nameenv, ty_dec, dec->u.func.dec);
            break;
        }
        default://todo array_proto func_id
            assert(0);
    }
    return ty_dec;
}

//trans Declator
static Ty_decList transDec(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_dec dec)
{
    Ty_decList decList;
    decList.head = decList.tail = NULL;
    if (dec == NULL)
        return decList;
    Ty_dec tail = checked_malloc(sizeof(*tail));
    switch(dec->kind)
    {
        case A_seq_dec:
            decList = transDec(level, linkenv, nameenv, dec->u.seq.dec);//seq
            tail = _transDec(level, linkenv, nameenv, tail, dec->u.seq.next);//not a seq
            decList = Ty_DecList(decList, tail);
            break;
        default:
            tail = _transDec(level, linkenv, nameenv, tail, dec);//not a seq
            decList = Ty_DecList(decList, tail);
            break;
    }
    return decList;
}

//translate declar in enum type
static int _transEDeclar(Tr_level level, E_linkage linkenv, E_namespace nameenv, int now, A_init init)
{
    switch (init->kind)
    {
        case A_seq_init:
            now = _transEDeclar(level, linkenv, nameenv, now, init->u.seq.init);
            now = _transEDeclar(level, linkenv, nameenv, now, init->u.seq.next);
            break;
        case A_enumtype_init:
        {
            //todo global scope
            E_enventry e = S_check(nameenv->venv, init->u.enumtype.id);
            if (e)
                EM_error(init->pos, "has been declared before");
            else
            {
                if (init->u.enumtype.const_exp)
                {
                    struct expty expty = transExp(level, linkenv, nameenv, init->u.enumtype.const_exp);
                    if (!(Ty_isIntTy(expty.ty) && Ty_isBasicCTy(expty.ty)))
                        EM_error(init->u.enumtype.const_exp->pos, "not a int constant");
                    else
                    {
                        e = E_VarEntry(Tr_ConstAccess(Tr_getIntConst(expty.exp)), Ty_Int());
                        S_enter(nameenv->venv, init->u.enumtype.id, e);
                        now = Tr_getIntConst(expty.exp) + 1;
                    }
                }
                else
                {
                    e = E_VarEntry(Tr_ConstAccess(now), Ty_Int());
                    S_enter(nameenv->venv, init->u.enumtype.id, e);
                    now = now + 1;
                }
            }
            break;
        }
        default:
            assert(0);
    }
    return now;
}

//translate enum type
static Ty_ty transEType(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_type type)
{
    Ty_ty resty;
    if (type->u.enumtype.init_list)//def
    {
        if (type->u.enumtype.id)
        {
            //todo if (nameenv->tenv->scope != S_FILE)
            resty = S_check(nameenv->tenv, type->u.enumtype.id);
            if (resty)
            {
                if (resty->complete)
                {
                    EM_error(type->pos, "has been declared before");
                    return resty;
                }
            }
            else
            {
                resty = Ty_ForwardTy(NULL);
                S_enter(nameenv->tenv, type->u.enumtype.id, resty);
            }

            _transEDeclar(level, linkenv, nameenv, 0, type->u.enumtype.init_list);

            if (!(resty->complete))
            {
                //todo check whith type to bind
                resty->u.forwardTy = Ty_VInt(0);
                resty->complete = 1;
                resty->size.exp = Tr_IntConst(SIZE_INT);
                resty->size.ty  = Ty_Int();//todo for all type size is UInt()
                resty->align = SIZE_INT;
            }
        }
    }
    else//declar
    {
        resty = S_check(nameenv->tenv, type->u.enumtype.id);
        if (!resty)
        {
            resty = Ty_ForwardTy(NULL);
            S_enter(nameenv->tenv, type->u.enumtype.id, resty);
        }
    }
    return resty;
}

//add variable to struct or union, calc size
static void _transSDeclar(Tr_level level, E_linkage linkenv, E_namespace nameenv, Ty_ty sty, A_declaration dec)
{
    switch(dec->kind)
    {
        case A_simple_declaration:
        {
            Ty_spec spec = transSpec(level, linkenv, nameenv, dec->u.simple.spec);
            Ty_dec  dec_ = transDec(level, linkenv, nameenv, dec->u.simple.dec).head;
            Ty_dec temp = dec_;
            while(temp)
            {
                //link type, update size and align
                Ty_dec declar = Ty_specdec(spec, temp);
                if (!declar->type->complete)//todo incomplete type
                {
                    EM_error(0,"incomplete type should not be a member");//todo pos
                    break;
                }
                //check multi declar and bind
                if (declar->sym)//is NULL only in bit type
                {
                    if (S_check(nameenv->menv, declar->sym))
                    {
                        EM_error(0,"has been declared before");//todo pos
                        break;
                    }
                    S_enter(nameenv->menv, declar->sym, declar->type);
                }
                //add into sty
                if (sty->kind == Ty_structTy)
                {
                    int temp_size = Tr_getIntConst(sty->size.exp);
                    //complete
                    sty->complete &= declar->type->complete;
                    //size
                    while(temp_size % declar->type->align)
                        temp_size += 1;
                    sty->u.structTy = Ty_SFieldList(sty->u.structTy, Ty_SField(declar->type, declar->sym, temp_size));
                    temp_size += Tr_getIntConst(declar->type->size.exp);
                    sty->size.exp = Tr_IntConst(temp_size);
                    //align
                    if (sty->u.structTy.head == NULL)
                        sty->align = declar->type->align;
                    
                }
                else//union
                {
                    int temp_size = Tr_getIntConst(sty->size.exp);
                    //complete
                    sty->complete &= declar->type->complete;
                    //size
                    if (Tr_getIntConst(declar->type->size.exp) > temp_size)
                        sty->size.exp = declar->type->size.exp;
                    //align
                    if (declar->type->align > sty->align)
                        sty->align = declar->type->align;
                    //structTy
                    sty->u.structTy = Ty_SFieldList(sty->u.structTy, Ty_SField(declar->type, declar->sym, 0));
                }
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
    Ty_ty resty = NULL;
    A_declaration dec_list = type->u.struct_union.dec_list;

    if (dec_list != NULL)//declaration
    {
        resty = checked_malloc(sizeof(*resty));
        resty->kind = type->u.struct_union.struct_union == A_STRUCT ? Ty_structTy : Ty_unionTy;
        
        //add forward declar
        if (type->u.struct_union.id)
        {
            Ty_ty ty = S_look(nameenv->tenv, type->u.struct_union.id);
            if (ty && (ty->kind != Ty_forwardTy || ty->complete))
                EM_error(type->pos, "has been declared before");
            if (ty == NULL)
            {
                //tenv id -> [forward] (NULL)
                Ty_ty ty = Ty_ForwardTy(NULL);
                S_enter(nameenv->tenv, type->u.struct_union.id, ty);
            }
        }
        
        nameenv = E_BeginScope(S_BLOCK, nameenv);
        _transSDeclar(level, linkenv, nameenv, resty, dec_list);
        nameenv = E_EndScope(nameenv);

        //link forward type
        if (type->u.struct_union.id)//has forward type
        {
            Ty_ty ty = S_look(nameenv->tenv, type->u.struct_union.id);
            ty->u.forwardTy = resty;
            ty->align = resty->align;
            ty->size = resty->size;
            ty->complete = resty->complete;
        }
    }
    else//reference
    {
        resty = S_look(nameenv->tenv, type->u.struct_union.id);
        if (resty == NULL)
        {
            //tenv id -> [forward] (NULL)
            Ty_ty ty = Ty_ForwardTy(NULL);
            S_enter(nameenv->tenv, type->u.struct_union.id, ty);
            resty = S_look(nameenv->tenv, type->u.struct_union.id);
        }
    }

    return resty;
}

static Tr_exp transInit(Tr_level level, E_linkage linkenv, E_namespace nameenv, E_enventry entry, A_init init)
{
    Tr_exp res = NULL;
    switch(init->kind)
    {
        case A_simple_init:
        {
            struct expty expty = transExp(level, linkenv, nameenv, init->u.simple);
            if (!Ty_canAssignTy(entry->u.var.ty, expty.ty))
            {
                EM_error(init->pos, "invalid init type");
                break;
            }
            res = Tr_assignExp(Tr_simpleVar(level, entry->u.var.access), expty.exp);
        }
        default://todo
            assert(0);
    }
    return res;
}

//may be called in file scope or in block scope
//alloc space immediately
static Tr_exp transDeclar(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_declaration declar)
{
    Tr_exp res = NULL;
    switch(declar->kind)
    {
        case A_seq_declaration:
        {
            Tr_exp exp1, exp2;
            exp1 = transDeclar(level, linkenv, nameenv, declar->u.seq.declaration);
            exp2 = transDeclar(level, linkenv, nameenv, declar->u.seq.next);
            res = Tr_seqStat(exp1, exp2);
            break;
        }
        case A_simple_declaration:
        {
            Ty_spec spec = transSpec(level, linkenv, nameenv, declar->u.simple.spec);
            Ty_decList  decList = transDec(level, linkenv, nameenv, declar->u.simple.dec);
            Ty_dec temp = decList.head;
            Ty_dec dec = NULL;
            while(temp)
            {
                dec = Ty_specdec(spec, temp);
                if (!dec->sym)//if is NULL, do nothing
                    goto next;

                if (!dec->type->complete)
                {
                    EM_error(declar->pos, "incomplete type");
                    goto next;
                }

                //todo check
                if (Ty_isTYPEDEF(spec->specs))//typedef
                {
                    if (S_check(nameenv->venv, dec->sym))
                    {
                        EM_error(declar->pos, "has been declared before");
                        goto next;
                    }
                    S_enter(nameenv->venv, dec->sym, dec->type);
                }
                else if (Ty_isEXTERN(spec->specs))//extern
                {
                    Ty_ty ty = NULL;
                    E_enventry entry = NULL;

                    if (!dec->type->complete)
                    {
                        EM_error(declar->pos, "incomplete type");
                        goto next;
                    }

                    ty = Ty_actualTy(dec->type);
                    entry = S_look(nameenv->venv, dec->sym);
                    
                    //todo better check
                    //has been declared before
                    if (entry)
                    {
                        if (dec->type->kind == Ty_funcTy)//func
                        {
                            if (entry->kind != E_funcEntry || Ty_areSameTy(entry->u.func.functy, ty))
                            {
                                EM_error(declar->pos, "declared as different type");
                                goto next;
                            }
                            if (!S_check(nameenv->venv, dec->sym))
                                S_enter(nameenv->venv, dec->sym, entry);
                        }
                        else
                        {
                            if (entry->kind != E_varEntry || Ty_areSameTy(entry->u.var.ty, ty))
                            {
                                EM_error(declar->pos, "declared as different type");
                                goto next;
                            }
                            if (!S_check(nameenv->venv, dec->sym))
                                S_enter(nameenv->venv, dec->sym, 
                                        E_VarEntry(entry->u.var.access, entry->u.var.ty));
                        }
                        
                    }
                    else//has not been declared before
                    {
                        if (dec->type->kind == Ty_funcTy)
                        {
                            entry = E_FuncEntry(dec->type, NULL);//has to be complete further
                            S_enter(linkenv->exlink, dec->sym, entry);
                            S_enter(nameenv->venv, dec->sym, entry);
                        }
                        else
                        {
                            if (dec->init)//todo init
                            {
                                EM_error(declar->pos, "extern should not be inited");
                                goto next;
                            }
                            Tr_access access = Tr_ExternAccess(dec->sym);
                            entry = E_VarEntry(access, dec->type);
                            S_enter(linkenv->exlink, dec->sym, entry);
                            S_enter(nameenv->venv, dec->sym, entry);
                        }
                    }
                }
                else if (Ty_isSTATIC(spec->specs))//static
                {
                    //todo
                }
                else
                {
                    Ty_ty ty = NULL;
                    E_enventry entry = NULL;

                    if (!dec->type->complete)
                    {
                        EM_error(declar->pos, "incomplete type");
                        goto next;
                    }

                    ty = Ty_actualTy(dec->type);
                    
                    if (ty->kind == Ty_funcTy)//function external linkage
                    {
                        //todo better check
                        entry = S_check(nameenv->venv, dec->sym);
                        if (entry)
                        {
                            EM_error(declar->pos, "has been declared before");
                            goto next;
                        }
                        entry = E_FuncEntry(dec->type, NULL);//has to be complete further
                        S_enter(linkenv->exlink, dec->sym, entry);
                        S_enter(nameenv->venv, dec->sym, entry);
                    }
                    else//variable
                    {
                        //todo better check
                        entry = S_check(nameenv->venv, dec->sym);
                        if (entry)
                        {
                            EM_error(declar->pos, "has been declared before");
                            goto next;
                        }
                        //alloc space
                        if (nameenv->venv->scope == S_FILE)
                        {
                            if (!(Ty_isIntTy(dec->type->size.ty) && Ty_isBasicCTy(dec->type->size.ty)))
                            {
                                EM_error(declar->pos, "size can not be determined");
                                goto next;  
                            }
                            Tr_access access = Tr_GlobalAccess(dec->type);
                            entry = E_VarEntry(access, dec->type);
                            S_enter(linkenv->exlink, dec->sym, entry);
                            S_enter(nameenv->venv, dec->sym, entry);
                        }
                        else
                        {
                            entry = E_VarEntry(NULL, dec->type);
                            S_enter(linkenv->nolink, dec->sym, entry);
                            S_enter(nameenv->venv, dec->sym, entry);
                            Tr_access access = Tr_StackAccess(level, dec->type);
                            entry = S_check(linkenv->nolink, dec->sym);
                            entry->u.var.access = access;
                            entry = S_check(nameenv->venv, dec->sym);
                            entry->u.var.access = access;                            
                        }
                        //init
                        if (dec->init)
                        {
                            res = transInit(level, linkenv, nameenv, entry, dec->init);
                        }
                    }
                        
                }
next:
                temp = temp->next;
            }
            break;
        }
        default:
            assert(0);
    }
    return res;
}

//add stat directly into level
static Tr_exp transStat(Tr_level level, E_linkage linkenv, E_namespace nameenv, Temp_label contl, Temp_label breakl, A_stat stat)
{
    Tr_exp res = NULL;
    switch(stat->kind)
    {
        case A_block_stat:
        {
            E_BeginScope(S_BLOCK, nameenv);
            linkenv->nolink = S_beginScope(S_NOLINK, linkenv->nolink);
            res = transStat(level, linkenv, nameenv, contl, breakl, stat->u.block);
            linkenv->nolink = S_endScope(linkenv->nolink);
            E_EndScope(nameenv);
            break;
        }
        case A_seq_stat:
        {
            Tr_exp exp1, exp2;
            exp1 = transStat(level, linkenv, nameenv, contl, breakl, stat->u.seq.stat);
            exp2 = transStat(level, linkenv, nameenv, contl, breakl, stat->u.seq.next);
            res = Tr_seqStat(exp1, exp2);
            break;
        }
        case A_exp_stat:
        {
            struct expty expty= transExp(level, linkenv, nameenv, stat->u.exp);
            if (expty.exp == NULL) break;
            res = expty.exp;
            break;
        }
        case A_dec_stat:
            transDeclar(level, linkenv, nameenv, stat->u.dec);
            break;
        case A_label_stat:
        {
            E_enventry entry = S_check(nameenv->lenv, stat->u.label.id);
            if (entry)
            {
                if (entry->u.label.complete)
                {
                    EM_error(stat->pos, "label statement defined before");
                    break;
                }

                res = Tr_labelStat(entry->u.label.label);
            }
            else
            {
                Temp_label label = Temp_namedlabel(stat->u.label.id->name);
                entry = E_LabelEntry(label, 1);
                S_enter(nameenv->lenv, stat->u.label.id, entry);
                res = Tr_labelStat(label);
            }
            break;
        }
        case A_gotostat_stat:
        {
            E_enventry entry;
            entry = S_look(nameenv->lenv, stat->u.gotostat);
            if (!entry)
            {
                Temp_label label = Temp_namedlabel(stat->u.gotostat->name);
                entry = E_LabelEntry(label, 0);
                S_enter(nameenv->lenv, stat->u.gotostat, label);
            }
            res = Tr_jumpStat(entry->u.label.label);
            break;
        }
        case A_continuestat_stat:
            if (contl == NULL)
            {
                EM_error(stat->pos, "continue is not in a loop");
                break;
            }
            res = Tr_jumpStat(contl);
            break;
        case A_breakstat_stat:
            if (breakl == NULL)
            {
                EM_error(stat->pos, "break is not in a loop");
                break;
            }
            res = Tr_jumpStat(breakl);
            break;
        case A_ifstat_stat:
        {
            Tr_exp cond = transExp(level, linkenv, nameenv, stat->u.ifstat.exp).exp;
            Tr_exp then = transStat(level, linkenv, nameenv, contl, breakl, stat->u.ifstat.stat_true);
            if (stat->u.ifstat.stat_false)
            {
                Tr_exp els = transStat(level, linkenv, nameenv, contl, breakl, stat->u.ifstat.stat_false);
                res = Tr_iftfStat(cond, then, els);
            }
            else
            {
                res = Tr_iftStat(cond, then);
            }
            break;
        }
        case A_whilestat_stat:
        {
            Tr_exp cond = transExp(level, linkenv, nameenv, stat->u.whilestat.exp).exp;
            Temp_label test = Temp_newlabel();
            Temp_label done = Temp_newlabel();
            Tr_exp body = transStat(level, linkenv, nameenv, test, done, stat->u.whilestat.stat);
            res = Tr_whileStat(test, cond, body, done);
            break;
        }
        case A_dowhile_stat:
        {
            Tr_exp cond = transExp(level, linkenv, nameenv, stat->u.dowhile.exp).exp;
            Temp_label bodyStart = Temp_newlabel();
            Temp_label done = Temp_newlabel();
            Tr_exp body = transStat(level, linkenv, nameenv, bodyStart, done, stat->u.whilestat.stat);
            res = Tr_dowhileStat(bodyStart, body, cond, done);
            break;
        }
        case A_forexp_stat:
        {
            if (stat->u.forexp.exp_2)
            {
                Tr_exp exp1 = transExp(level, linkenv, nameenv, stat->u.forexp.exp_1).exp;
                Tr_exp exp2 = transExp(level, linkenv, nameenv, stat->u.forexp.exp_2).exp;
                Tr_exp exp3 = transExp(level, linkenv, nameenv, stat->u.forexp.exp_3).exp;
                Temp_label test = Temp_newlabel();
                Temp_label done = Temp_newlabel();
                Tr_exp body = transStat(level, linkenv, nameenv, test, done, stat->u.forexp.stat);
                res = Tr_forexpcondStat(exp1, test, exp2, body, exp3, done);
                break;
            }
            else
            {
                Tr_exp exp1 = transExp(level, linkenv, nameenv, stat->u.forexp.exp_1).exp;
                Tr_exp exp3 = transExp(level, linkenv, nameenv, stat->u.forexp.exp_3).exp;
                Temp_label test = Temp_newlabel();
                Temp_label done = Temp_newlabel();
                Tr_exp body = transStat(level, linkenv, nameenv, test, done, stat->u.forexp.stat);
                res = Tr_forexpStat(exp1, test, body, exp3, done);
                break;
            }
            assert(0);
        }
        case A_returnstat_stat:
        {
            struct expty returnExpty = transExp(level, linkenv, nameenv, stat->u.returnstat);
            if (!Ty_canAssignTy(level->functy->u.funcTy.returnTy, returnExpty.ty))
            {
                EM_error(stat->pos, "return type isn't match");
                break;
            }
            res = Tr_returnStat(level, returnExpty.exp);
            break;
        }
        default:
            assert(0);
        /*
        A_casestat_stat,
        A_defaultstat_stat,
        A_switchstat_stat,
        A_fordec_stat,
        */
    }
    return res;
}

static Tr_exp transDef(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_def def)
{
    Tr_exp res;
    switch(def->kind)
    {
        case A_seq_def:
        {
            Tr_exp exp1, exp2;
            exp1 = transDef(level, linkenv, nameenv, def->u.seq.def);
            exp2 = transDef(level, linkenv, nameenv, def->u.seq.next);
            res = Tr_seqStat(exp1, exp2);
            break;
        }
        case A_func_def:
        {
            Tr_level newlevel = NULL;
            E_enventry entry = NULL;
            Ty_spec spec = transSpec(level, linkenv, nameenv, def->u.func.spec);
            Ty_decList decList = transDec(level, linkenv, nameenv, def->u.func.func);
            Ty_dec dec = decList.head;

            dec = Ty_specdec(spec, dec);
            if (!dec->sym)
                break;
            entry = S_check(nameenv->venv, dec->sym);
            if (entry)
            {
                if (entry->kind != E_funcEntry || !Ty_areSameTy(entry->u.func.functy, dec->type))
                {
                    EM_error(def->pos, "has been declared as different type");
                    break;
                }
                if (entry->u.func.level)
                {
                    EM_error(def->pos, "has been defined before");
                    break;
                }
                newlevel = Tr_newLevel(level, Temp_newlabel(), dec->type);
                entry->u.func.level = newlevel;
            }
            else
            {
                newlevel = Tr_newLevel(level, Temp_newlabel(), dec->type);
                entry = E_FuncEntry(dec->type, newlevel);
            }

            Ty_field temp = NULL;
            Tr_access access = NULL;
            S_symbol labelName = NULL;
            Tr_exp stats = NULL;
            nameenv->lenv = S_beginScope(S_FUNC, nameenv->lenv);
            for(temp = dec->type->u.funcTy.params.head; temp; temp = temp->next)
            {
                access = Tr_StackAccess(newlevel, temp->ty);
                entry = E_VarEntry(access, temp->ty);
                S_enter(nameenv->venv, dec->sym, entry);
                S_enter(linkenv->nolink, dec->sym, entry);
            }
            stats = transStat(newlevel, linkenv, nameenv, NULL, NULL, def->u.func.stat);
            Tr_procEntryExit(newlevel, stats);
            if ((labelName = E_checkLabel(nameenv->lenv)))
                EM_error(0, "Label %s not found", labelName->name);//todo pos
            nameenv->lenv = S_endScope(nameenv->lenv);
            break;
        }
        case A_simple_dec:
            res = transDeclar(level, linkenv, nameenv, def->u.simple);
            break;
        default:
            assert(0);
    }
    return res;
}

static exptyList transFuncArgs(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_exp args)
{
    exptyList list;
    list.head = list.tail = NULL;
    switch(args->kind)
    {
        case A_seq_exp:
        {
            struct expty arg;
            list = transFuncArgs(level, linkenv, nameenv, args->u.seq.expr);
            arg = transExp(level, linkenv, nameenv, args->u.seq.next);
            list = SE_ExptyList(list, SE_ExptyNode(arg));
            break;
        }
        default:
        {
            struct expty arg;
            arg = transExp(level, linkenv, nameenv, args);
            list = SE_ExptyList(list, SE_ExptyNode(arg));
            break;
        }
    }
    return list;
}

struct expty transExp(Tr_level level, E_linkage linkenv, E_namespace nameenv, A_exp a)
{
    struct expty res;
    res = Expty(NULL, Ty_Void());
    switch (a->kind){
        case A_id_exp :
        {
            E_enventry e = S_look(nameenv->venv, a->u.id);
            if (e == NULL)
                EM_error(a->pos, "No such variable");
            if (e->kind == E_varEntry)
                res = Expty(Tr_simpleVar(level, e->u.var.access), e->u.var.ty);
            else if (e->kind == E_funcEntry)
                res = Expty(Tr_simpleFunc(e->u.func.level), e->u.func.functy);
            else
                assert(0);
            break;
        }
        case A_intexp_exp :
            res = Expty(Tr_IntConst(a->u.intexp), Ty_Int());
            break;
        case A_floatexp_exp :
            res = Expty(Tr_DoubleConst(a->u.floatexp), Ty_Double());
            break;
        case A_charexp_exp :
            res = Expty(Tr_CharConst(a->u.charexp), Ty_Char());
            break;
        case A_strexp_exp :
            res = Expty(Tr_strExp(a->u.strexp), Ty_PointerTy(Ty_Char(), Ty_CONST));
            break;
        case A_subscript_exp : 
        {
            struct expty idExp = transExp(level, linkenv, nameenv, a->u.subscript.expr);
            struct expty sbExp = transExp(level, linkenv, nameenv, a->u.subscript.subscript);
            if (!Ty_isIntTy(sbExp.ty)){
                EM_error(a->pos, "Subscript needs to be int type");
                break;
            }
            if (Ty_isArrayTy(idExp.ty))
            {
                Ty_ty subty = idExp.ty->u.arrayTy.ty;
                Ty_calcASC(idExp.ty);
                if (!subty->complete)
                {
                    EM_error(a->pos, "subscript type is incomplete");
                    break;
                }
                res = Expty(Tr_subExp(idExp.exp, sbExp.exp, subty->size.exp), subty);
            }
            else if (Ty_isPointerTy(idExp.ty))
            {
                Ty_ty subty = idExp.ty->u.pointerTy.ty;
                Ty_calcASC(idExp.ty);
                if (!subty->complete)
                {
                    EM_error(a->pos, "subscript type is incomplete");
                    break;
                }
                res = Expty(Tr_subExp(idExp.exp, sbExp.exp, subty->size.exp), subty);
            }
            else
            {
                EM_error(a->pos, "id needs to be array or pointer type");
                break;
            }
            break;
        }
        case A_funccall_exp : //Simple One
        {
            Ty_field        field;
            exptyList       args;
            exptyNode       arg, temp;
            Tr_expList      expList;
            struct expty    funcExp;
            Ty_ty           returnTy;

            funcExp = transExp(level, linkenv, nameenv, a->u.funccall.expr);
            if (Ty_isFuncTy(funcExp.ty))
            {
                field = funcExp.ty->u.funcTy.params.head;
                returnTy = funcExp.ty->u.funcTy.returnTy;
            }
            else if (Ty_isPointerTy(funcExp.ty) && Ty_isFuncTy(funcExp.ty->u.pointerTy.ty))//function pointer
            {
                field = funcExp.ty->u.pointerTy.ty->u.funcTy.params.head;
                returnTy = funcExp.ty->u.pointerTy.ty->u.funcTy.returnTy;
            }
            else
            {
                EM_error(a->pos, "func is not able to be called");
                break;
            }
            
            expList.head = expList.tail = NULL;
            args = transFuncArgs(level, linkenv, nameenv, a->u.funccall.args);

            arg = args.head;
            while(field && arg)
            {
                if (!Ty_canAssignTy(field->ty, arg->expty.ty))
                {
                    EM_error(a->pos, "not consistent param type");//todo pos
                    break;
                }
                expList = Tr_ExpList(expList, arg->expty.exp);
                field = field->next;
                temp = arg;
                arg = arg->next;
                free(temp);
            }

            if (field != NULL || arg != NULL)
            {
                EM_error(a->pos, "arg num is different");
                break;
            }

            returnTy->specs |= Ty_RVAL | Ty_CONST;
            res = Expty(Tr_funcCallExp(funcExp.exp, expList), returnTy);
            break;
        }
        case A_dot_exp :
        {
            struct expty structExp;
            Ty_sField mem = NULL;

            structExp = transExp(level, linkenv, nameenv, a->u.dot.expr);
            if (!Ty_isSUTy(structExp.ty))
            {
                EM_error(a->pos, "type is not struct");
                break;
            }

            mem = structExp.ty->u.structTy.head;
            while(mem)
            {
                if (mem->name == a->u.dot.id)
                    break;
                mem = mem->next;
            }
            if (mem == NULL)
            {
                EM_error(a->pos, "member not found");
                break;
            }
            res = Expty(Tr_dotExp(structExp.exp, mem->offset), mem->ty);
            break;
        }
        case A_point_exp ://->
        {
            struct expty spExp;//struct pointer
            Ty_sField mem = NULL;

            spExp = transExp(level, linkenv, nameenv, a->u.point.expr);
            if (!Ty_isPointerTy(spExp.ty) || !Ty_isSUTy(spExp.ty->u.pointerTy.ty))
            {
                EM_error(a->pos, "type is not a pointer to struct");
                break;
            }

            mem = spExp.ty->u.pointerTy.ty->u.structTy.head;
            while(mem)
            {
                if (mem->name == a->u.point.id)
                    break;
                mem = mem->next;
            }
            if (mem == NULL)
            {
                EM_error(a->pos, "member not found");
                break;
            }
            res = Expty(Tr_pointExp(spExp.exp, mem->offset), mem->ty);
            break;
        }
        case A_postpp_exp : case A_postmm_exp : 
        {
            struct expty expty = transExp(level, linkenv, nameenv, a->u.postpp);
            if (Ty_isRealTy(expty.ty))
            {
                if (!Ty_isMLTy(expty.ty))
                {
                    EM_error(a->pos, "++ can not applied to const or rvalue");
                    break;
                }
                if (a->kind == A_postpp_exp)
                    res = Expty(Tr_postppIntExp(expty.exp), expty.ty);
                else
                    res = Expty(Tr_postmmIntExp(expty.exp), expty.ty);
            }
            else if (Ty_isPointerTy(expty.ty))
            {
                if (!Ty_isMLTy(expty.ty))
                {
                    EM_error(a->pos, "++ can not applied to const or rvalue");
                    break;
                }
                if (a->kind == A_postpp_exp)
                    res = Expty(Tr_postppPointerExp(expty.exp, expty.ty->u.pointerTy.ty->size.exp), expty.ty);
                else
                    res = Expty(Tr_postmmPointerExp(expty.exp, expty.ty->u.pointerTy.ty->size.exp), expty.ty);
            }
            else
            {
                EM_error(a->pos, "Only integer or pointer can operate ++");
                break;
            }
            break;
        }
        
        case A_init_exp : //unsupported
        {
            break;
        }

        case A_prepp_exp : case A_premm_exp :
        {
            struct expty expty = transExp(level, linkenv, nameenv, a->u.postpp);
            if (Ty_isIntTy(expty.ty))
            {
                if (!Ty_isMLTy(expty.ty) || !Ty_isMLTy(expty.ty))
                {
                    EM_error(a->pos, "++ can not applied to const or rvalue");
                    break;
                }
                if (a->kind == A_prepp_exp)
                    res = Expty(Tr_preppIntExp(expty.exp), expty.ty);
                else
                    res = Expty(Tr_premmIntExp(expty.exp), expty.ty);
            }
            else if (Ty_isPointerTy(expty.ty))
            {
                if (!Ty_isMLTy(expty.ty) || !Ty_isMLTy(expty.ty))
                {
                    EM_error(a->pos, "++ can not applied to const or rvalue");
                    break;
                }
                if (a->kind == A_prepp_exp)
                    res = Expty(Tr_preppPointerExp(expty.exp, expty.ty->u.pointerTy.ty->size.exp), expty.ty);
                else
                    res = Expty(Tr_premmPointerExp(expty.exp, expty.ty->u.pointerTy.ty->size.exp), expty.ty);
            }
            else
            {
                EM_error(a->pos, "Only integer or pointer can operate ++");
                break;
            }
            break;
        }
        case A_unaryop_exp : 
        {
            //A_AND, A_MUL, A_PLUS, A_MINUS, A_NOT, A_LOGNOT;
            struct expty expty = transExp(level, linkenv, nameenv, a->u.unaryop.expr);
            switch (a->u.unaryop.op)
            {
                case A_AND:
                {
                    if (!Ty_canGetPointer(expty.ty))
                    {
                        EM_error(a->pos, "can not get address of this type");
                        break;
                    }
                    else if (Ty_isRVAL(expty.ty->specs))
                    {
                        EM_error(a->pos, "can not get address of rval");
                        break;
                    }
                    res = Expty(Tr_getPointer(expty.exp), Ty_PointerTy(expty.ty, 0));
                    break;
                }
                case A_MUL:
                {
                    if (!Ty_isPointerTy(expty.ty))
                    {
                        EM_error(a->pos, "type is not pointer");
                        break;
                    }
                    res = Expty(Tr_getMem(expty.exp), expty.ty->u.pointerTy.ty);
                    break;
                }
                case A_PLUS:
                    if (!Ty_isArithTy(expty.ty))
                    {
                        EM_error(a->pos, "type is not arithmetic type");
                        break;
                    }
                    break;
                case A_MINUS:
                {
                    if (!Ty_isArithTy(expty.ty))
                    {
                        EM_error(a->pos, "type is not arithmetic type");
                        break;
                    }
                    if (Ty_isBasicCTy(expty.ty))
                        res = Expty(Tr_CBinop(Tr_MULINT, expty.exp, Tr_IntConst(-1)), expty.ty);
                    else
                    {
                        if (Ty_isIntTy(expty.ty))
                            res = Expty(Tr_Binop(Tr_MULINT, expty.exp, Tr_IntConst(-1)), expty.ty);
                        else
                        {
                            EM_error(a->pos,"double is not supported");
                            break;
                            //res = Expty(Tr_mulDouble(expty.exp, Tr_IntConst(-1)), expty.ty);
                        }
                    }
                    break;
                }
                case A_NOT:
                {
                    if (!Ty_isIntTy(expty.ty))
                    {
                        EM_error(a->pos, "type is not int type");
                        break;
                    }
                    if (Ty_isBasicCTy(expty.ty))
                        res = Expty(Tr_notConst(expty.exp), expty.ty);
                    else
                        res = Expty(Tr_notInt(expty.exp), expty.ty);
                }
                case A_LOGNOT:
                {
                    if (!Ty_isScalarTy(expty.ty))
                    {
                        EM_error(a->pos, "type is not scalar type");
                        break;
                    }
                    if (Ty_isBasicCTy(expty.ty))
                        res = Expty(Tr_lognotConst(expty.exp), expty.ty);
                    else
                        res = Expty(Tr_lognotInt(expty.exp), Ty_VInt(Ty_CONST | Ty_RVAL));
                    break;
                }
                default:
                    assert(0);
            }
            break;
        }
        case A_sizeof_unary_exp : 
        {
            struct expty expty = transExp(level, linkenv, nameenv, a->u.sizeof_unary);
            if (!Ty_isCompleteTy(expty.ty) || Ty_isFuncTy(expty.ty))//todo bit
            {
                EM_error(a->pos, "sizeof can not applied to uncomplete, func type");
                break;
            }
            res = Expty(Tr_sizeof(expty.ty), Ty_Int());
            break;
        }
        case A_sizeof_type_exp ://TODO
        {
            break;
        }

        case A_cast_exp : {//TODO
            break;
        }
        
        case A_binop_exp : //only for integer
        {
            struct expty left = transExp(level, linkenv, nameenv, a->u.binop.expr1);
            struct expty right= transExp(level, linkenv, nameenv, a->u.binop.expr2);
            Ty_ty returnTy = NULL;
            switch (a->u.binop.op){//only the same type
                case A_MUL : case A_DIV : 
                {
                    if (!Ty_isArithTy(left.ty) || !Ty_isArithTy(right.ty)){
                        EM_error(a->pos, "inconsistent type to mul or div");
                        break;
                    }
                    returnTy = Ty_muldivTy(left.ty, right.ty);
                    if (Ty_isIntTy(returnTy))
                    {
                        if (Ty_isBasicCTy(returnTy))
                        {
                            if (a->u.binop.op == A_MUL)
                                res = Expty(Tr_CBinop(Tr_MULINT, left.exp, right.exp), returnTy);
                            else
                                res = Expty(Tr_CBinop(Tr_DIVINT, left.exp, right.exp), returnTy);
                        }
                        else
                        {
                            if (a->u.binop.op == A_MUL)
                                res = Expty(Tr_Binop(Tr_MULINT, left.exp, right.exp), returnTy);
                            else
                                res = Expty(Tr_Binop(Tr_DIVINT, left.exp, right.exp), returnTy);
                        }
                    }
                    else
                    {
                        //double
                    }
                    break;
                }
                case A_MOD : 
                {
                    if (!Ty_isIntTy(left.ty) || !Ty_isIntTy(right.ty)){
                        EM_error(a->pos, "inconsistent type to mod");
                        break;
                    }
                    returnTy = Ty_modTy(left.ty, right.ty);
                    if (Ty_isBasicCTy(returnTy))
                        res = Expty(Tr_CBinop(Tr_MODINT, left.exp, right.exp), returnTy);
                    else
                        res = Expty(Tr_Binop(Tr_MODINT, left.exp, right.exp), returnTy);
                    break;
                }
                case A_PLUS : case A_MINUS : 
                {
                    if ((Ty_isArithTy(left.ty) && (Ty_isArithTy(right.ty) || Ty_isPointerTy(right.ty)))
                     ||(Ty_isArithTy(right.ty) && (Ty_isArithTy(right.ty) || Ty_isPointerTy(left.ty))))
                    {
                        returnTy = Ty_plusminusTy(left.ty, right.ty);
                        if (Ty_isIntTy(returnTy) || Ty_isPointerTy(returnTy))
                        {
                            if (Ty_isBasicCTy(returnTy))
                            {
                                if (a->u.binop.op == A_PLUS)
                                    res = Expty(Tr_CBinop(Tr_PLUSINT, left.exp, right.exp), returnTy);
                                else
                                    res = Expty(Tr_CBinop(Tr_SUBINT, left.exp, right.exp), returnTy);
                            }
                            else
                            {
                                if (a->u.binop.op == A_PLUS)
                                    res = Expty(Tr_Binop(Tr_PLUSINT, left.exp, right.exp), returnTy);
                                else
                                    res = Expty(Tr_Binop(Tr_SUBINT, left.exp, right.exp), returnTy);
                            }
                        }
                        else
                        {
                            //double
                        }
                    }
                    else
                    {
                        EM_error(a->pos, "inconsistent type to add or minus");
                        break;
                    }
                    break;
                }
                case A_LSHIFT : case A_RSHIFT:
                {
                    if (!Ty_isIntTy(left.ty) || !Ty_isIntTy(right.ty)){
                        EM_error(a->pos, "inconsistent type to shift");
                        break;
                    }
                    returnTy = Ty_lrshiftTy(left.ty, right.ty);
                    if (Ty_isBasicCTy(returnTy))
                    {
                        if (a->u.binop.op == A_LSHIFT)
                            res = Expty(Tr_CBinop(Tr_LSINT, left.exp, right.exp), returnTy);
                        else
                            res = Expty(Tr_CBinop(Tr_RSINT, left.exp, right.exp), returnTy);
                    }
                    else
                    {
                        if (a->u.binop.op == A_LSHIFT)
                            res = Expty(Tr_Binop(Tr_LSINT, left.exp, right.exp), returnTy);
                        else
                            res = Expty(Tr_Binop(Tr_RSINT, left.exp, right.exp), returnTy);
                    }
                    break;
                }
                case A_GT : case A_GTE : case A_LT : case A_LTE :
                {
                    if (
                    (Ty_isArithTy(left.ty) && Ty_isArithTy(right.ty)) ||
                    (Ty_isPointerTy(left.ty) && Ty_isPointerTy(right.ty) && 
                        Ty_areSameTy(left.ty->u.pointerTy.ty, right.ty->u.pointerTy.ty))
                    )
                    {
                        returnTy = Ty_VInt(Ty_RVAL);
                        if ((Ty_isIntTy(left.ty) || Ty_isPointerTy(left.ty)) &&
                            (Ty_isIntTy(right.ty) || Ty_isPointerTy(right.ty))
                        )
                        {
                            if (Ty_isBasicCTy(returnTy))
                            {
                                if (a->u.binop.op == A_GT)
                                    res = Expty(Tr_CRelop(Tr_GTINT, left.exp, right.exp), returnTy);
                                if (a->u.binop.op == A_LT)
                                    res = Expty(Tr_CRelop(Tr_LTINT, left.exp, right.exp), returnTy);
                                if (a->u.binop.op == A_GTE)
                                    res = Expty(Tr_CRelop(Tr_GTEINT, left.exp, right.exp), returnTy);
                                if (a->u.binop.op == A_LTE)
                                    res = Expty(Tr_CRelop(Tr_LTEINT, left.exp, right.exp), returnTy);
                            }
                            else
                            {
                                if (a->u.binop.op == A_GT)
                                    res = Expty(Tr_Relop(Tr_GTINT, left.exp, right.exp), returnTy);
                                if (a->u.binop.op == A_LT)
                                    res = Expty(Tr_Relop(Tr_LTINT, left.exp, right.exp), returnTy);
                                if (a->u.binop.op == A_GTE)
                                    res = Expty(Tr_Relop(Tr_GTEINT, left.exp, right.exp), returnTy);
                                if (a->u.binop.op == A_LTE)
                                    res = Expty(Tr_Relop(Tr_LTEINT, left.exp, right.exp), returnTy);
                            }
                        }
                        else
                        {
                            //double
                        }
                    }
                    else
                    {
                        EM_error(a->pos, "inconsistent type to inequality");
                        break;
                    }
                    break;
                }
                case A_EQ : case A_NEQ :
                {
                    if (
                    (Ty_isArithTy(left.ty) && Ty_isArithTy(right.ty)) ||
                    (Ty_isPointerTy(left.ty) && Ty_isPointerTy(right.ty) && 
                        (Ty_areSameTy(left.ty->u.pointerTy.ty, right.ty->u.pointerTy.ty) ||
                         (Ty_isVoidTy(left.ty->u.pointerTy.ty) || Ty_isVoidTy(right.ty->u.pointerTy.ty)) ||
                         (left.ty == Ty_Null() || right.ty == Ty_Null())
                        ))
                    )
                    {
                        returnTy = Ty_VInt(0);
                        if ((Ty_isIntTy(left.ty) || Ty_isPointerTy(left.ty)) &&
                            (Ty_isIntTy(right.ty) || Ty_isPointerTy(right.ty))
                        )
                        {
                            if (Ty_isBasicCTy(left.ty) && Ty_isBasicCTy(right.ty))
                            {
                                if (a->u.binop.op == A_EQ)
                                    res = Expty(Tr_CRelop(Tr_EQINT, left.exp, right.exp), returnTy);
                                if (a->u.binop.op == A_NEQ)
                                    res = Expty(Tr_CRelop(Tr_NEQINT, left.exp, right.exp), returnTy);
                            }
                            else
                            {
                                if (a->u.binop.op == A_GT)
                                    res = Expty(Tr_Relop(Tr_EQINT, left.exp, right.exp), returnTy);
                                if (a->u.binop.op == A_LT)
                                    res = Expty(Tr_Relop(Tr_NEQINT, left.exp, right.exp), returnTy);
                            }
                        }
                    }
                    else
                    {
                        EM_error(a->pos, "inconsistent type to equality");
                        break;
                    }
                    break;
                }
                case A_AND : case A_XOR : case A_OR : 
                {
                    if (!Ty_isIntTy(left.ty) || !Ty_isIntTy(right.ty)){
                        EM_error(a->pos, "inconsistent type to bitwise op");
                        break;
                    }
                    returnTy = Ty_bitwiseTy(left.ty, right.ty);
                    if (Ty_isBasicCTy(returnTy))
                    {
                        if (a->u.binop.op == A_AND)
                            res = Expty(Tr_CBinop(Tr_ANDINT, left.exp, right.exp), returnTy);
                        else if (a->u.binop.op == A_XOR)
                            res = Expty(Tr_CBinop(Tr_XORINT, left.exp, right.exp), returnTy);
                        else
                            res = Expty(Tr_CBinop(Tr_ORINT, left.exp, right.exp), returnTy);
                    }
                    else
                    {
                        if (a->u.binop.op == A_AND)
                            res = Expty(Tr_Binop(Tr_ANDINT, left.exp, right.exp), returnTy);
                        else if (a->u.binop.op == A_XOR)
                            res = Expty(Tr_Binop(Tr_XORINT, left.exp, right.exp), returnTy);
                        else
                            res = Expty(Tr_Binop(Tr_ORINT, left.exp, right.exp), returnTy);
                    }
                    break;
                }
                case A_LOGAND : case A_LOGOR:
                {
                    if (!Ty_isScalarTy(left.ty) || !Ty_isScalarTy(right.ty)){
                        EM_error(a->pos, "inconsistent type to bitwise op");
                        break;
                    }
                    returnTy = Ty_logicTy(left.ty, right.ty);
                    if (Ty_isBasicCTy(returnTy))
                    {
                        if (a->u.binop.op == A_LOGAND)
                            res = Expty(Tr_CRelop(Tr_LOGANDINT, left.exp, right.exp), returnTy);
                        else
                            res = Expty(Tr_CRelop(Tr_LOGORINT, left.exp, right.exp), returnTy);
                    }
                    else
                    {
                        if (a->u.binop.op == A_LOGAND)
                            res = Expty(Tr_Relop(Tr_LOGANDINT, left.exp, right.exp), returnTy);
                        else
                            res = Expty(Tr_Relop(Tr_LOGORINT, left.exp, right.exp), returnTy);
                    }
                    break;
                }
                default:
                    assert(0);
            }
            break;
        }
        
        case A_threeop_exp : //todo
        {
            break;
        }

        case A_assign_exp : 
        {
            switch(a->u.assign.op)
            {
                case A_ASSIGN:
                {
                    struct expty src = transExp(level, linkenv, nameenv, a->u.assign.src);
                    struct expty dst = transExp(level, linkenv, nameenv, a->u.assign.dst);
                    if (!Ty_canAssignTy(dst.ty, src.ty))
                    {
                        EM_error(a->pos, "type inconsistent when assign");
                        break;
                    }
                    if (!Ty_isMLTy(dst.ty))
                    {
                        EM_error(a->pos, "can only assign to modifiable lval");
                        break;
                    }
                    res = Expty(Tr_assignExp(dst.exp, src.exp), dst.ty);//todo RVAL
                    break;
                }
                default://todo
                    assert(0);
            }
            break;
        }
        case A_seq_exp : 
        {
            struct expty expty1 = transExp(level, linkenv, nameenv, a->u.seq.expr);
            struct expty expty2 = transExp(level, linkenv, nameenv, a->u.seq.next);
            res = Expty(Tr_seqExp(expty1.exp, expty2.exp), expty2.ty);
            break;
        }
        default:
            assert(0);
    }
    return res;
}

F_fragList SEM_transProg(A_def def)
{
    transDef(Tr_Outermost(), E_Linkage(), E_Namespace(), def);
	return Tr_getResult();
}
