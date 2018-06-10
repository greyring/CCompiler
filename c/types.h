#ifndef _TYPES_H_
#define _TYPES_H_
//used for type checking and help to allocate mem

#include "absync.h"
#include "symbol.h"
#include "translate.h"

typedef struct Ty_spec_ *Ty_spec;
typedef struct Ty_dec_ *Ty_dec;//a list of dec
typedef struct Ty_decList_ Ty_decList;//notice this is a struct
typedef struct Ty_ty_ *Ty_ty;
typedef struct Ty_field_ *Ty_field;//a list of field
typedef struct Ty_fieldList_ Ty_fieldList;//a struct
typedef struct Ty_sField_ *Ty_sField;//a list of struct field
typedef struct Ty_sFieldList_ Ty_sFieldList;//a struct

struct Ty_expty
{
    Tr_exp exp;
    Ty_ty ty;
};

struct Ty_spec_
{
    unsigned long specs;
    int LONG;
    Ty_ty complex_type;
};

struct Ty_dec_
{
    A_init   init;
    Ty_ty    type;
    S_symbol sym;
    Ty_dec   next;
};

struct Ty_decList_
{
    Ty_dec head;
    Ty_dec tail;
};

//for param
struct Ty_field_
{
    Ty_ty ty;
    S_symbol name;
    Ty_field next;
};

struct Ty_fieldList_
{
    Ty_field head;
    Ty_field tail;
};

//for struct and union
struct Ty_sField_
{
    Ty_ty ty;
    S_symbol name;
    int offset;
    Ty_sField next;
};

struct Ty_sFieldList_
{
    Ty_sField head;
    Ty_sField tail;
};

struct Ty_ty_
{
    unsigned long specs;//only contains storage spec, qual, func type
    int align;
    struct Ty_expty size;//for variable length array
    int complete;//default to 0
    enum{
        Ty_forwardTy,//forward declar for struct union func ... , different from nameTy
        Ty_nameTy,
        Ty_basicTy,
        Ty_structTy,//struct
        Ty_unionTy,
        Ty_bitTy,
        Ty_pointerTy,
        Ty_arrayTy,
        Ty_funcTy,
    }kind;
    union{
        Ty_ty nameTy, forwardTy;
        Ty_ty basicTy;//because basic may have specs so...
        Ty_sFieldList structTy;
        struct
        {
            Ty_ty ty;
            int bitSize;
        }bitTy;
        struct
        {
            Ty_ty ty;
            unsigned long qual;
        }pointerTy;
        struct
        {
            Ty_ty ty;
            struct Ty_expty constExp;
        }arrayTy;
        struct{
            Ty_ty returnTy;
            Ty_fieldList params;
        }funcTy;
    }u;
};

#define DECLAR(type, num) \
enum {Ty_##type = (unsigned long)1<<num}; \
unsigned long Ty_is##type(unsigned long specs);

//qualifier
DECLAR(CONST, 0); DECLAR(VOLATILE, 1); DECLAR(RESTRICT, 2);
//storage type
DECLAR(TYPEDEF, 3); DECLAR(EXTERN, 4); DECLAR(STATIC, 5); DECLAR(AUTO, 6); DECLAR(REGISTER, 7);
//func type
DECLAR(INLINE, 8);
//prim type
DECLAR(VOID, 9);
DECLAR(CHAR, 10); DECLAR(SHORT, 11); DECLAR(INT, 12);
DECLAR(FLOAT, 13); DECLAR(DOUBLE, 14);
DECLAR(SIGNED, 15); DECLAR(UNSIGNED, 16);
DECLAR(STRUCT, 17); DECLAR(UNION, 18); DECLAR(ENUM, 19);

DECLAR(RVAL, 20);//can not get address
#undef DECLAR

Ty_ty Ty_Null();
Ty_ty Ty_Void();    
Ty_ty Ty_Char();    Ty_ty Ty_VChar(unsigned long);
Ty_ty Ty_UChar();   Ty_ty Ty_VUChar(unsigned long);
Ty_ty Ty_Short();   Ty_ty Ty_VShort(unsigned long);
Ty_ty Ty_UShort();  Ty_ty Ty_VUShort(unsigned long);
Ty_ty Ty_Int();     Ty_ty Ty_VInt(unsigned long);
Ty_ty Ty_UInt();    Ty_ty Ty_VUInt(unsigned long);
Ty_ty Ty_Long();    Ty_ty Ty_VLong(unsigned long);
Ty_ty Ty_ULong();   Ty_ty Ty_VULong(unsigned long);
Ty_ty Ty_LLong();   Ty_ty Ty_VLLong(unsigned long);
Ty_ty Ty_ULLong();  Ty_ty Ty_VULLong(unsigned long);
Ty_ty Ty_Float();   Ty_ty Ty_VFloat(unsigned long);
Ty_ty Ty_Double();  Ty_ty Ty_VDouble(unsigned long);
Ty_ty Ty_LDouble(); Ty_ty Ty_VLDouble(unsigned long);

//test spec
unsigned long Ty_isLONG(Ty_spec spec);
unsigned long Ty_isSimpleType(Ty_spec spec);

//test type
int Ty_isVoidTy(Ty_ty ty);//todo
int Ty_isBasicCTy(Ty_ty ty);//todo
int Ty_isIntTy(Ty_ty ty);//todo
int Ty_isRealTy(Ty_ty ty);//todo
int Ty_isArithTy(Ty_ty ty);//todo
int Ty_isScalarTy(Ty_ty ty);//todo
int Ty_isArrayTy(Ty_ty ty);//todo
int Ty_isPointerTy(Ty_ty ty);//todo
int Ty_isFuncTy(Ty_ty ty);//todo
int Ty_isSUTy(Ty_ty ty);//todo
int Ty_isMLTy(Ty_ty ty);//modifieable lval

int Ty_isCompleteTy(Ty_ty ty);//todo


void Ty_calcASC(Ty_ty type);
Ty_dec Ty_specdec(Ty_spec spec, Ty_dec dec);


Ty_ty Ty_ForwardTy(Ty_ty ty);//init incomplete
Ty_ty Ty_NameTy(Ty_ty ty);
Ty_ty Ty_BitTy(Ty_ty ty, int bitSize);
Ty_ty Ty_PointerTy(Ty_ty ty, unsigned long qual);
Ty_ty Ty_ArrayTy(Ty_ty ty, Tr_exp constExp, Ty_ty expTy);
Ty_ty Ty_FuncTy(Ty_ty returnTy, Ty_fieldList params);

Ty_decList Ty_DecList(Ty_decList head, Ty_dec tail);//return a struct
Ty_field Ty_Field(Ty_ty ty, S_symbol name);
Ty_fieldList Ty_FieldList(Ty_fieldList head, Ty_field tail);
Ty_sField Ty_SField(Ty_ty ty, S_symbol name, int offset);
Ty_sFieldList Ty_SFieldList(Ty_sFieldList head, Ty_sField tail);//return a struct

Ty_ty Ty_actualTy(Ty_ty ty);

//semant type test
//actual ty
int Ty_areSameTy(Ty_ty ty1, Ty_ty ty2);//todo
int Ty_canAssignTy(Ty_ty dst, Ty_ty src);//todo
int Ty_canGetPointer(Ty_ty ty);//todo

Ty_ty Ty_muldivTy(Ty_ty ty1, Ty_ty ty2);//todo
Ty_ty Ty_modTy(Ty_ty ty1, Ty_ty ty2);//todo
Ty_ty Ty_plusminusTy(Ty_ty ty1, Ty_ty ty2);//todo
Ty_ty Ty_lrshiftTy(Ty_ty ty1, Ty_ty ty2);//todo
Ty_ty Ty_bitwiseTy(Ty_ty ty1, Ty_ty ty2);//todo
Ty_ty Ty_logicTy(Ty_ty ty1, Ty_ty ty2);//todo

#endif