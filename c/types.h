#ifndef _TYPES_H_
#define _TYPES_H_
//no qual
//used only for type checking
//size and align are not included

#include "absync.h"
#include "symbol.h"
#include "translate.h"


typedef struct Ty_spec_ *Ty_spec;
typedef struct Ty_dec_ *Ty_dec;
typedef struct Ty_decList_ *Ty_decList;
typedef struct Ty_ty_ *Ty_ty;
typedef struct Ty_field_ *Ty_field;
typedef struct Ty_fieldList_ *Ty_fieldList;
typedef struct Ty_sField_ *Ty_sField;
typedef struct Ty_sFieldList_ * Ty_sFieldList;

struct Ty_spec_
{
    unsigned long specs;
    int LONG;
    Ty_ty complex_type;
};

struct Ty_dec_
{
    A_init    init;
    Ty_ty    type;
    S_symbol sym;
    Ty_dec   next;
};

struct Ty_decList_
{
    Ty_dec head;
    Ty_decList tail;
};

struct Ty_ty_
{
    unsigned long specs;
    int align, size;
    enum{
        Ty_name,
        Ty_basic,
        Ty_structTy,//struct and union
        Ty_bitTy,
        Ty_pointerTy,
        Ty_arrayTy,
        Ty_funcTy,
    }kind;
    union{
        Ty_ty name;
        Ty_SFieldList structTy;
        struct
        {
            Ty_ty ty;
            A_exp constExp;
        }bitTy;
        struct
        {
            Ty_ty ty;
            unsigned long qual;
        }pointerTy;
        struct
        {
            Ty_ty ty;
            A_exp constExp;
        }arrayTy;
        struct{
            Ty_ty returnTy;
            Ty_fieldList params;
        }funcTy;
    }u;
};

//for param
struct Ty_field_
{
    Ty_ty ty;
    S_symbol name;
};

struct Ty_fieldList_
{
    Ty_field head;
    Ty_fieldList tail;
};

//for struct and union
struct Ty_sField_
{
    Ty_ty ty;
    S_symbol name;
    int offset;
};

struct Ty_sFieldList_
{
    Ty_sField head;
    Ty_sFieldList tail;
};

#define DECLAR(NAME) \
extern const unsigned long Ty_##NAME; \
unsigned long Ty_is##NAME(Ty_spec spec);

DECLAR(CONST); DECLAR(VOLATILE); DECLAR(RESTRICT);
DECLAR(TYPEDEF); DECLAR(EXTERN); DECLAR(STATIC); DECLAR(AUTO); DECLAR(REGISTER);
DECLAR(INLINE);
DECLAR(VOID); DECLAR(CHAR); DECLAR(SHORT); DECLAR(INT); DECLAR(INT); DECLAR(LONG);
DECLAR(FLOAT); DECLAR(DOUBLE); DECLAR(SIGNED); DECLAR(UNSIGNED); 
DECLAR(STRUCT); DECLAR(UNION); DECLAR(ENUM);

unsigned long Ty_isSimpleType(Ty_spec spec);

Ty_ty Ty_Void();
Ty_ty Ty_Char();
Ty_ty Ty_UChar();
Ty_ty Ty_Short();
Ty_ty Ty_UShort();
Ty_ty Ty_Int();
Ty_ty Ty_UInt();
Ty_ty Ty_Long();
Ty_ty Ty_ULong();
Ty_ty Ty_LLong();
Ty_ty Ty_ULLong();
Ty_ty Ty_Float();
Ty_ty Ty_Double();
Ty_ty Ty_LDouble();

Ty_ty Ty_ConstP();




Ty_dec Ty_DecList(Ty_dec head, Ty_dec tail);
Ty_ty Ty_BitTy(Ty_ty ty, A_exp constExp);
Ty_ty Ty_PointerTy(Ty_ty ty, unsigned long qual);
Ty_ty Ty_ArrayTy(Ty_ty ty, A_exp constExp);
Ty_ty Ty_FuncTy(Ty_ty returnTy, Ty_fieldList params);

Ty_field Ty_Field(Ty_ty ty, S_symbol name);
Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail);
Ty_sField Ty_SField(Ty_ty ty, S_symbol name, int offset);
Ty_sFieldList Ty_SFieldList(Ty_sField head, Ty_SFieldList tail);

#endif