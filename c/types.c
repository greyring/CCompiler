#include "limits.h"
#include "types.h"

#define SPEC(type, num) \
const unsigned long Ty_##type = 1<<num; \
unsigned long Ty_is##type(Ty_spec spec)\
{\
    return (spec->specs & Ty_##type);\
}

//qualifier
SPEC(CONST, 0); SPEC(VOLATILE, 1); SPEC(RESTRICT, 2);
//storage type
SPEC(TYPEDEF, 3); SPEC(EXTERN, 4); SPEC(STATIC, 5); SPEC(AUTO, 6); SPEC(REGISTER, 7);
//func type
SPEC(INLINE, 8);
//prim type
SPEC(VOID, 9);
SPEC(CHAR, 10); SPEC(SHORT, 11); SPEC(INT, 12);
SPEC(FLOAT, 13); SPEC(DOUBLE, 14);
SPEC(SIGNED, 15); SPEC(UNSIGNED, 16);
SPEC(STRUCT, 17); SPEC(UNION, 18); SPEC(ENUM, 19);
#undef SPEC

unsigned long Ty_isLONG(Ty_spec spec)
{
    return spec->LONG != 0;
}

unsigned long Ty_isSimpleType(Ty_spec ty_spec)
{
    return Ty_isVOID(ty_spec) || Ty_isCHAR(ty_spec) || Ty_isSHORT(ty_spec) || Ty_isINT(ty_spec)
            ||Ty_isLONG(ty_spec) || Ty_isFLOAT(ty_spec) || Ty_isDOUBLE(ty_spec)
            ||Ty_isSIGNED(ty_spec) || Ty_isUNSIGNED(ty_spec);
}

#define BASIC(NAME, ALIGN, SIZE, SIGN) \
struct Ty_ty_ Ty_##NAME##_ = \
{.kind = Ty_basic, \
 .align = ALIGN, \
 .size = SIZE, \
 .sign = SIGN, \
 .incomplete = 0,\
 };\
Ty_ty Ty_##NAME (void){return &Ty_##NAME##_;}

struct Ty_ty_ Ty_VOID_ = 
{
    .kind = Ty_basic,
    .align = 0,
    .sign = 0,
    .sign = -1,
    .incomplete = 1
};
BASIC(Char,   SIZE_CHAR, SIZE_CHAR, 1);
BASIC(UChar,  SIZE_CHAR, SIZE_CHAR, 0);
BASIC(Short,  SIZE_SHORT, SIZE_SHORT, 1);
BASIC(UShort, SIZE_SHORT, SIZE_SHORT, 0);
BASIC(Int,    SIZE_INT, SIZE_INT, 1);
BASIC(UInt,   SIZE_INT, SIZE_INT, 0);
BASIC(Long,   SIZE_LONG, SIZE_LONG, 1);
BASIC(ULong,  SIZE_LONG, SIZE_LONG, 0);
BASIC(LLong,  SIZE_LONGLONG, SIZE_LONGLONG, 1);
BASIC(ULLong, SIZE_LONGLONG, SIZE_LONGLONG, 0);
BASIC(Float,  SIZE_FLOAT, SIZE_FLOAT, 1);
BASIC(Double, SIZE_DOUBLE, SIZE_DOUBLE, 1);
BASIC(LDouble, SIZE_LONGDOUBLE, SIZE_LONGDOUBLE, 1);
#undef BASIC

static int Ty_isBasic(Ty_spec spec)
{
    return (Ty_isVOID(spec) || Ty_isCHAR(spec) || Ty_isSHORT(spec) || Ty_isINT(spec) || Ty_isLONG(spec) ||
        Ty_isUNSIGNED(spec) ||  Ty_isSIGNED(spec) ||
        Ty_isDOUBLE(spec) || Ty_isFLOAT(spec));
}

static void Ty_linkTy(Ty_ty outer, Ty_ty inner)
{
    switch (outer->kind)
    {
        case Ty_name:
            if (outer->u.name)
                Ty_linkTy(outer->u.name, inner);
            else
                outer->u.name = inner;
            break;
        case Ty_bitTy:
            if (outer->u.bitTy.ty)//todo check
                assert(0);
            else
                outer->u.bitTy.ty = inner;
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
}

//merge spec with the first node in dec
Ty_dec Ty_specdec(Ty_spec spec, Ty_dec dec)
{
    unsigned long specs;
    int LONG;
    Ty_ty complex_type;
    if (spec->complex_type)
    {
        Ty_linkTy(dec->type, spec->complex_type);
        dec->type->specs = spec->specs;
    }
    else if (Ty_isBasic(spec))
    {

    }
    else
    {

    }
}

Ty_ty Ty_BitTy(Ty_ty ty, A_exp constExp)//todo
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->align = -1;
    p->size = -1;
    p->sign = -1;
    p->incomplete = 0;
    p->kind = Ty_bitTy;
    p->u.bitTy.ty = ty;
    p->u.bitTy.constExp = constExp;//todo
    return p;
}

Ty_ty Ty_PointerTy(Ty_ty ty, unsigned long qual)
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->align = SIZE_ALIGN;
    p->size = SIZE_POINTER;
    p->sign = -1;
    p->kind = Ty_pointerTy;
    p->incomplete = 0;
    p->u.pointerTy.ty = ty;
    p->u.pointerTy.qual = qual;
    return p;
}

Ty_ty Ty_ArrayTy(Ty_ty ty, A_exp constExp)//todo
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->align = SIZE_ALIGN;
    if (ty)
        p->size = ty->size;//todo
    p->sign = -1;
    p->incomplete = !constExp;    
    p->kind = Ty_arrayTy;
    p->u.arrayTy.ty = ty;
    p->u.arrayTy.constExp = constExp;
    return p;
}

Ty_ty Ty_FuncTy(Ty_ty returnTy, Ty_fieldList params)
{
    Ty_ty p = checked_malloc(sizeof(*p));
    p->align = SIZE_ALIGN;
    p->size = -1;
    p->sign = -1;
    p->incomplete = 1;    
    p->kind = Ty_funcTy;
    p->u.funcTy.returnTy = returnTy;
    p->u.funcTy.params = params;
    return p;
}



Ty_dec Ty_DecList(Ty_dec head, Ty_dec tail)
{
    head->next = tail;
    return head;
}

Ty_field Ty_Field(Ty_ty ty, S_symbol name)
{
    Ty_field p = checked_malloc(sizeof(*p));
    p->ty = ty;
    p->name = name;
    return p;
}

Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail)
{
    Ty_fieldList p = checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

Ty_sField Ty_SField(Ty_ty ty, S_symbol name, int offset)
{
    Ty_SField p = checked_malloc(sizeof(*p));
    p->ty = ty;
    p->name = name;
    p->offset = offset;
    return p;
}

Ty_sFieldList Ty_SFieldList(Ty_sField head, Ty_SFieldList tail)
{
    Ty_SFieldList p = checked_malloc(sizeof(*p));
    p->head = head;
    p->tail = tail;
    return p;
}

/*
//一个星号，不需要参数ty
static struct Ty_ty_ typointer = {Ty_pointer};
Ty_ty Ty_Pointer(void){
    return &typointer;
}


Ty_ty Ty_Name(S_symbol sym, Ty_ty ty){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_name;
    t->u.name.name = sym;
    t->u.name.type = ty;

    return t;
}

Ty_ty Ty_Array(Ty_ty ty){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_array;
    t->u.array.array = ty;
    t->u.array.num = -1;

    return t;
}

Ty_ty Ty_Array(Ty_ty ty, int num){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_array;
    t->u.array.array = ty;
    t->u.array.num = num;

    return t;
}


Ty_ty Ty_Struct(Ty_fieldList fields){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_struct;
    t->u.struct_ty = fields;

    return t;
}

Ty_ty Ty_Union(Ty_FieldList fields){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_union;
    t->u.union_ty = fields;

    return t;
}

Ty_ty Ty_Enum(Ty_ty ty){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_enum;
    t->u.enum_ty = ty;

    return t;
}

Ty_ty Ty_Decpointer(Ty_ty type, Ty_ty pointer){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_decpointer;
    t->u.decpointer.type = type;
    t->u.decpointer.pointer = pointer;

    return t;
}

Ty_ty Ty_Constant(Ty_ty type){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_constant;
    t->u.constant_ty = type;

    return t;
}



Ty_field Ty_Field(S_symbol name, Ty_ty ty){
    Ty_field t = checked_malloc(sizeof(*t));
    t->type = ty;
    t->name = name;
    return t;
}

Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail){
    Ty_fieldList t = checked_malloc(sizeof(*t));
    t->head = head;
    t->tail = tail;
    return t;
};

Ty_tyList Ty_TyList(Ty_ty head, Ty_tyList tail){
    Ty_tyList t = checked_malloc(sizeof(*t));
    t->head = head;
    t->tail = tail;
    return &t;
}
*/

