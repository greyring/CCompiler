#include "limits.h"
#include "types.h"
#include "errormsg.h"
#include "translate.h"
#include <assert.h>

static struct Ty_expty Ty_Expty(Tr_exp exp, Ty_ty ty)
{
    struct Ty_expty t;
    t.exp = exp;
    t.ty = ty;
}

#define SPEC(type) \
unsigned long Ty_is##type(unsigned long specs)\
{\
    return (specs & Ty_##type);\
}

//qualifier
SPEC(CONST); SPEC(VOLATILE); SPEC(RESTRICT);
//storage type
SPEC(TYPEDEF); SPEC(EXTERN); SPEC(STATIC); SPEC(AUTO); SPEC(REGISTER);
//func type
SPEC(INLINE);
//prim type
SPEC(VOID);
SPEC(CHAR); SPEC(SHORT); SPEC(INT);
SPEC(FLOAT); SPEC(DOUBLE);
SPEC(SIGNED); SPEC(UNSIGNED);
SPEC(STRUCT); SPEC(UNION); SPEC(ENUM);
SPEC(RVAL);//can not get address
#undef SPEC

#define BASIC(NAME) \
struct Ty_ty_ Ty_##NAME##_ =\
{.kind = Ty_basicTy, \
 .specs = Ty_CONST | Ty_RVAL,\
 .complete = 0, \
 .u.basicTy = NULL, \
 };\
Ty_ty Ty_##NAME (void){return &Ty_##NAME##_;}\
Ty_ty Ty_V##NAME (unsigned long spec)\
{\
    Ty_ty p = checked_malloc(sizeof(*p));\
    p->kind = Ty_basicTy;\
    p->complete = 0;\
    p->u.basicTy = Ty_##NAME();\
}

BASIC(Null);
BASIC(Void)
BASIC(Char); BASIC(UChar);
BASIC(Short); BASIC(UShort);
BASIC(Int); BASIC(UInt);
BASIC(Long); BASIC(ULong);
BASIC(LLong); BASIC(ULLong);
BASIC(Float);
BASIC(Double); BASIC(LDouble);
#undef BASIC

//test a spec
unsigned long Ty_isLONG(Ty_spec spec)
{
    return spec->LONG != 0;
}

//test a spec is simple type
unsigned long Ty_isSimpleType(Ty_spec ty_spec)
{
    return Ty_isVOID(ty_spec->specs) || Ty_isCHAR(ty_spec->specs) || Ty_isSHORT(ty_spec->specs) || Ty_isINT(ty_spec->specs)
            ||Ty_isLONG(ty_spec) || Ty_isFLOAT(ty_spec->specs) || Ty_isDOUBLE(ty_spec->specs)
            ||Ty_isSIGNED(ty_spec->specs) || Ty_isUNSIGNED(ty_spec->specs);
}

int Ty_isIntTy(Ty_ty ty)
{
    if (ty == NULL) 
        return 0;
    if (ty == Ty_Int() || ty == Ty_UInt())//int const such as 123
        return 1;
    switch(ty->kind)//todo check
    {
        case Ty_nameTy:
            return Ty_isIntTy(ty->u.nameTy);
        case Ty_basicTy:
            return Ty_isIntTy(ty->u.basicTy);
    }
    return 0;
}

int Ty_isIntCTy(Ty_ty ty)
{
    if (ty == NULL)
        return 0;
    if (ty == Ty_Int() || ty == Ty_UInt())
        return 1;
    return 0;
}

int Ty_isVoidTy(Ty_ty ty){
    if (ty == NULL)
        return 0;
    ty = Ty_actualTy(ty);
    if (ty == Ty_Void())
        return 1;
    else
        return 0;
}

int Ty_isBasicCTy(Ty_ty ty){
    if (ty == NULL)
        return 0;
    ty = Ty_actualTy(ty);
    if (ty == Ty_Char() || ty == Ty_UChar() || ty == Ty_Short()
        || ty == Ty_UShort() || ty == Ty_Int() || ty == Ty_UInt()
        || ty == Ty_Long() || ty == Ty_ULong() || ty == Ty_LLong()
        || ty == Ty_ULLong() || ty == Ty_Float() || ty == Ty_Double()
        || ty == Ty_LDouble())
        return 1;
    else
        return 0;
}

int Ty_isRealTy(Ty_ty ty){
    if (ty == NULL)
        return 0;
    ty = Ty_actualTy(ty);
    if (Ty_isIntTy(ty) == 1 || ty == Ty_Float() || ty == Ty_Double()
        || ty == Ty_LDouble())
        return 1;
    else
        return 0;
}

int Ty_isArithTy(Ty_ty ty){
    if (ty == NULL)
        return 0;
    ty = Ty_actualTy(ty);
    if (Ty_isIntTy(ty) == 1 || Ty_isRealTy(ty) == 1)
        return 1;
    else
        return 0;
}

int Ty_isScalarTy(Ty_ty ty){
    if (ty == NULL)
        return 0;
    ty = Ty_actualTy(ty);
    if (Ty_isArithTy(ty) == 1 || Ty_isPointerTy(ty) == 1)
        return 1;
    else
        return 0;
}

int Ty_isArrayTy(Ty_ty ty){
    if (ty == NULL)
        return 0;
    ty = Ty_actualTy(ty);
    if (ty->kind == Ty_arrayTy)
        return 1;
    else
        return 0;
}

int Ty_isPointerTy(Ty_ty ty){
    if (ty == NULL)
        return 0;
    ty = Ty_actualTy(ty);
    if (ty->kind == Ty_pointerTy)
        return 1;
    else
        return 0;
}

int Ty_isFuncTy(Ty_ty ty){
    if (ty == NULL)
        return 0;
    ty = Ty_actualTy(ty);
    if (ty->kind == Ty_funcTy)
        return 1;
    else
        return 0;
}

int Ty_isSUTy(Ty_ty ty){
    if (ty == NULL)
        return 0;
    ty = Ty_actualTy(ty);
    if (ty->kind == Ty_structTy || ty->kind == Ty_unionTy)
        return 1;
    else
        return 0;
}

//modifieable lval
int Ty_isMLTy(Ty_ty ty)
{
    if (ty == NULL)
        return 0;
    ty = Ty_actualTy(ty);
    if (Ty_isCONST(ty->specs) || Ty_isRVAL(ty->specs))
        return 0;
    return 1;
}

int Ty_isCompleteTy(Ty_ty ty)
{
    if (ty == NULL)
        return 0;
    Ty_calcASC(ty);
    return ty->complete;
}



//translate spec of basic type into basic Ty
static Ty_ty Ty_BasicTy(Ty_spec spec)
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->kind = Ty_basicTy;
    p->specs = spec->specs;

    //carefully coding
    int sign = 0, unsign = 0, isint = 0;
    if (Ty_isSIGNED(spec->specs))
        sign = 1;
    else if (Ty_isUNSIGNED(spec->specs))
        unsign = 1;

    if (Ty_isINT(spec->specs))
        isint = 1;

    //void
    if (Ty_isVOID(spec->specs))
    {
        p->u.basicTy = Ty_Void();
        goto done;
    }
    //float
    if (Ty_isFLOAT(spec->specs))
    {
        p->u.basicTy = Ty_Float();
        goto done;
    }
    //double longdouble
    if (Ty_isDOUBLE(spec->specs))
    {
        if (spec->LONG == 1)
            p->u.basicTy = Ty_LDouble();
        else p->u.basicTy = Ty_Double();
        goto done;
    }
    //char uchar
    if (Ty_isCHAR(spec->specs))
    {
        if (unsign) p->u.basicTy = Ty_UChar();
        else p->u.basicTy = Ty_Char();
        goto done;
    }
    //short ushort
    if (Ty_isSHORT(spec->specs))
    {
        if (unsign) p->u.basicTy = Ty_UShort();
        else p->u.basicTy = Ty_Short();
        goto done;
    }
    //long ulong llong ullong
    if (spec->LONG)
    {
        //long ulong
        if (spec->LONG == 1)
        {
            if (unsign) p->u.basicTy = Ty_ULong();
            else p->u.basicTy = Ty_Long();
        }
        //longlong ulonglong
        else
        {
            if (unsign) p->u.basicTy = Ty_ULLong();
            else p->u.basicTy = Ty_LLong();
        }
        goto done;
    }
    //int uint
    if (isint)
    {
        if (unsign) p->u.basicTy = Ty_UInt();
        else p->u.basicTy = Ty_Int();
        goto done;
    }
    assert(0);    
done:
    return p;
}

//link bit, pointer, array, func with basic or complex types
static Ty_ty Ty_linkTy(Ty_ty outer, Ty_ty inner)
{
    if (outer == NULL)
        return inner;
    
    switch (outer->kind)
    {
        case Ty_bitTy:
            if (outer->u.bitTy.ty)//todo check
                assert(0);
            else
            {
                if (!Ty_isIntTy(inner))
                    EM_error(0, "bit field's type should be int or unsigned int");//todo pos
                outer->u.bitTy.ty = inner;
            }
            break;
        case Ty_pointerTy:
            if (outer->u.pointerTy.ty)
                Ty_linkTy(outer->u.pointerTy.ty, inner);
            else
                outer->u.pointerTy.ty = inner;
            break;
        case Ty_arrayTy:
            if (outer->u.arrayTy.ty)
                Ty_linkTy(outer->u.arrayTy.ty, inner);
            else
                outer->u.arrayTy.ty = inner;
            break;
        case Ty_funcTy:
            if (outer->u.funcTy.returnTy)
                Ty_linkTy(outer->u.funcTy.returnTy, inner);
            else
                outer->u.funcTy.returnTy = inner;
            break;
        default:
            assert(0);
    }
    return outer;
}

//calc align size complete
//not handle forward declar
//only complete type has size
//todo size is UIntTy
void Ty_calcASC(Ty_ty type)
{
    //has been calc before
    if (type->complete)
        return;
    switch(type->kind)
    {
        case Ty_forwardTy:
            return;
        case Ty_nameTy:
            Ty_calcASC(type->u.nameTy);
            type->complete = type->u.nameTy->complete;
            type->align = type->u.nameTy->align;
            if (type->complete)
                type->size = type->u.nameTy->size;
            break;
        case Ty_basicTy://must complete
            //prim type will only be init once
            if (type->u.basicTy == NULL)
            {
                if (type == Ty_Void())
                {
                    type->complete = 1;
                    type->align = 0;
                    type->size = Ty_Expty(Tr_IntConst(0), Ty_Int());
                }
                else if (type == Ty_Char() || type == Ty_UChar())
                {
                    type->complete = 1;
                    type->align = SIZE_CHAR;
                    type->size = Ty_Expty(Tr_IntConst(SIZE_CHAR), Ty_Int());
                }
                else if (type == Ty_Short() || type == Ty_UShort())
                {
                    type->complete = 1;
                    type->align = SIZE_SHORT;
                    type->size = Ty_Expty(Tr_IntConst(SIZE_SHORT), Ty_Int());
                }
                else if (type == Ty_Int() || type == Ty_UInt())
                {
                    type->complete = 1;
                    type->align = SIZE_INT;
                    type->size = Ty_Expty(Tr_IntConst(SIZE_INT), Ty_Int());
                }
                else if (type == Ty_Long() || type == Ty_ULong())
                {
                    type->complete = 1;
                    type->align = SIZE_LONG;
                    type->size = Ty_Expty(Tr_IntConst(SIZE_LONG), Ty_Int());
                }
                else if (type == Ty_LLong() || type == Ty_ULLong())
                {
                    type->complete = 1;
                    type->align = SIZE_LONGLONG;
                    type->size = Ty_Expty(Tr_IntConst(SIZE_LONGLONG), Ty_Int());
                }
                else if (type == Ty_Float())
                {
                    type->complete = 1;
                    type->align = SIZE_FLOAT;
                    type->size = Ty_Expty(Tr_IntConst(SIZE_FLOAT), Ty_Int());
                }
                else if (type == Ty_Double())
                {
                    type->complete = 1;
                    type->align = SIZE_DOUBLE;
                    type->size = Ty_Expty(Tr_IntConst(SIZE_DOUBLE), Ty_Int());
                }
                else if (type == Ty_LDouble())
                {
                    type->complete = 1;
                    type->align = SIZE_LONGDOUBLE;
                    type->size = Ty_Expty(Tr_IntConst(SIZE_LONGDOUBLE), Ty_Int());
                }
                else
                    assert(0);
            }
            else
            {
                type->complete = 1;
                type->size = type->u.basicTy->size;
                type->align = type->u.basicTy->align;
            }
            break;
        case Ty_structTy://should be handled when declar not here
            break;
        case Ty_unionTy://should be handled when declar not here
            break;
        case Ty_bitTy://todo not support
            break;
        case Ty_pointerTy:
            Ty_calcASC(type->u.pointerTy.ty);
            type->complete = 1;
            type->align = SIZE_POINTER;
            type->size = Ty_Expty(Tr_IntConst(SIZE_POINTER), Ty_Int());
            break;
        case Ty_arrayTy:
            Ty_calcASC(type->u.arrayTy.ty);
            if (type->u.arrayTy.constExp.exp)
            {
                type->complete = 1;
                if (Ty_isIntCTy(type->u.arrayTy.constExp.ty) && 
                    Ty_isIntCTy(type->u.arrayTy.ty->size.ty))
                    type->size = Ty_Expty(Tr_mulConst(type->u.arrayTy.constExp.exp, 
                                                         type->u.arrayTy.ty->size.exp),
                                          Ty_Int());
                else
                {
                    if (Ty_isIntTy(type->u.arrayTy.constExp.ty))
                        type->size = Ty_Expty(Tr_mulInt(type->u.arrayTy.constExp.exp,
                                                        type->u.arrayTy.ty->size.exp),
                                              Ty_VInt(0));
                    else
                        EM_error(0,"array size is not integer");//todo pos
                }
            }
            else
                type->complete = 0;
            type->align = type->u.arrayTy.ty->align;
        case Ty_funcTy:
            type->complete = 1;
            type->size = Ty_Expty(Tr_IntConst(0), Ty_Int());
            type->align = 0;
            break;
        default:
            assert(0);
    }
}

//dec is a single dec, link type, add spec, calc align size incomplete
Ty_dec Ty_specdec(Ty_spec spec, Ty_dec dec)
{
    //link type and add spec
    if (spec->complex_type)
    {
        spec->complex_type->specs = spec->specs;
        dec->type = Ty_linkTy(dec->type, spec->complex_type);
    }
    else if (Ty_isSimpleType(spec))
        dec->type = Ty_linkTy(dec->type, Ty_BasicTy(spec));
    else
        assert(0);

    Ty_calcASC(dec->type);
    return dec;
}

//a forward declar, init incomplete
Ty_ty Ty_ForwardTy(Ty_ty ty)
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->kind = Ty_forwardTy;
    p->u.forwardTy = ty;
    p->complete = 0;
    return p;
}

//a typedef
Ty_ty Ty_NameTy(Ty_ty ty)
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->kind = Ty_nameTy;
    p->u.nameTy = ty;
    return p;
}

Ty_ty Ty_BitTy(Ty_ty ty, int bitSize)//todo
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->kind = Ty_bitTy;
    p->u.bitTy.ty = ty;
    p->u.bitTy.bitSize = bitSize;//todo
    return p;
}

Ty_ty Ty_PointerTy(Ty_ty ty, unsigned long qual)
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->kind = Ty_pointerTy;
    p->u.pointerTy.ty = ty;
    p->u.pointerTy.qual = qual;
    return p;
}

Ty_ty Ty_ArrayTy(Ty_ty ty, Tr_exp constExp, Ty_ty expTy)//todo
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->kind = Ty_arrayTy;
    p->u.arrayTy.ty = ty;
    p->u.arrayTy.constExp = Ty_Expty(constExp, expTy);
    return p;
}

Ty_ty Ty_FuncTy(Ty_ty returnTy, Ty_fieldList params)
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->kind = Ty_funcTy;
    p->u.funcTy.returnTy = returnTy;
    p->u.funcTy.params = params;
    return p;
}



Ty_decList Ty_DecList(Ty_decList list, Ty_dec tail)
{
    if (list.head == NULL)
    {
        list.head = list.tail = tail;
    }
    else
    {
        list.tail->next = tail;
        list.tail = tail;
    }
    return list;
}

Ty_field Ty_Field(Ty_ty ty, S_symbol name)
{
    Ty_field p = checked_malloc(sizeof(*p));
    p->ty = ty;
    p->name = name;
    return p;
}

Ty_fieldList Ty_FieldList(Ty_fieldList head, Ty_field tail)
{
    if (!head.head)
        head.head = head.tail = tail;
    else
        head.tail = head.tail->next = tail;
    return head;
}

Ty_sField Ty_SField(Ty_ty ty, S_symbol name, int offset)
{
    Ty_sField p = checked_malloc(sizeof(*p));
    p->ty = ty;
    p->name = name;
    p->offset = offset;
    return p;
}

Ty_sFieldList Ty_SFieldList(Ty_sFieldList head, Ty_sField tail)
{
    if (!head.head)
        head.head = head.tail = tail;
    else
        head.tail = head.tail->next = tail;
    return head;
}

Ty_ty Ty_actualTy(Ty_ty ty)
{
    if (ty->kind == Ty_forwardTy)
        return Ty_actualTy(ty->u.forwardTy);
    else if (ty->kind == Ty_nameTy)
        return Ty_actualTy(ty->u.nameTy);
    return ty;
}

int Ty_areSameTy(Ty_ty ty1, Ty_ty ty2)
{
    ty1 = Ty_actualTy(ty1);
    ty2 = Ty_actualTy(ty2);
    if (ty1 == ty2)
        return 1;
    if (ty1->kind != ty2->kind)
        return 0;
    if (Ty_isCONST(ty1->specs) != Ty_isCONST(ty2->specs))
        return 0;
    switch(ty1->kind)
    {
        case Ty_basicTy:
            if (ty1->u.basicTy == ty2->u.basicTy)
                return 1;
            else
                return 0;
        case Ty_structTy:
        case Ty_unionTy:
        {
            Ty_sField temp1, temp2;
            temp1 = ty1->u.structTy.head;
            temp2 = ty2->u.structTy.head;
            while(temp1 && temp2)
            {
                if (temp1->name != temp2->name)
                    return 0;
                if (!Ty_areSameTy(temp1->ty, temp2->ty))
                    return 0;
                temp1 = temp1->next;
                temp2 = temp2->next;
            }
            if (temp1 == temp2)
                return 1;
            return 0;
        }
        case Ty_bitTy://todo
            if (!Ty_areSameTy(ty1->u.bitTy.ty, ty2->u.bitTy.ty))
                return 0;
            return 1;
        case Ty_pointerTy:
            return Ty_areSameTy(ty1->u.pointerTy.ty, ty2->u.pointerTy.ty);
        case Ty_arrayTy://todo check
            if (!Ty_areSameTy(ty1->u.arrayTy.ty, ty2->u.arrayTy.ty))
                return 0;
            if (ty1->u.arrayTy.constExp.ty == ty2->u.arrayTy.constExp.ty)
                return 1;
            return 0;
        case Ty_funcTy:
        {
            Ty_field temp1, temp2;
            if (!Ty_areSameTy(ty1->u.funcTy.returnTy, ty2->u.funcTy.returnTy))
                return 0;
            temp1 = ty1->u.funcTy.params.head;
            temp2 = ty2->u.funcTy.params.head;
            while(temp1 && temp2)
            {
                if (temp1->name != temp2->name)
                    return 0;
                if (!Ty_areSameTy(temp1->ty, temp2->ty))
                    return 0;
            }
            if (temp1 == temp2)
                return 1;
            return 0;
        }
        default:
            assert(0);
    }
    assert(0);
}

int Ty_canAssignTy(Ty_ty dst, Ty_ty src)
{
    //todo
}