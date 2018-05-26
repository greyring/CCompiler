#ifndef _TYPES_H_
#define _TYPES_H_
//no qual
//used only for type checking
//size and align are not included

#include "symbol.h"

typedef struct Ty_ty_ *Ty_ty;
typedef struct Ty_field_ *Ty_field;
typedef struct Ty_fieldList_ *Ty_fieldList;
typedef struct Ty_tyList_ *Ty_tyList;

/*future
typedef struct
{
    char CONST:1    = 0;
    char VOLATILE:1 = 0;
    char RESTRICT:1 = 0;
}Ty_qual_;
*/

struct Ty_ty_
{
    //Ty_qual_ qual;
    enum{
        Ty_basic_ty,
        Ty_name_ty,
        Ty_array_ty,
        Ty_func_ty,
        Ty_structty_ty,
        Ty_pointer_ty,
        //Ty_unionty_ty,
        //enum is a list of constant int defination
        //enum itself is an int
    }kind;
    union{
        struct{
            S_symbol name;
            Ty_ty    type;
        }name;
        struct{
            Ty_ty array;
            int     num;//-1:not specified
        }array;//for 2 arrays, if num is given, they should be same
        Ty_ty pointer;
        Ty_fields structty_ty;
    }u;
};

struct Ty_field_
{
    //Ty_qual_ qual;
    //A_exp     bit;//may be null
    //Ty_fields next;
    Ty_ty    type;
    S_symbol name;//may be null
    int    offset;
    int    size;
    int    align;
};

struct Ty_fieldList_{
    Ty_field head;
    Ty_fieldList tail;
};

struct Ty_tyList_{
    Ty_ty head;
    Ty_tyList tail;
};

Ty_ty Ty_VOID(void);
Ty_ty Ty_INT(void);
/*
Ty_ty Ty_SCHAR(void);//todo qual
Ty_ty Ty_UCHAR(void);//todo qual
Ty_ty Ty_SSHORT(void);//todo qual
Ty_ty Ty_USHORT(void);//todo qual
Ty_ty Ty_SINT(void);//todo qual
Ty_ty Ty_UINT(void);//todo qual
Ty_ty Ty_SLONG(void);//todo qual
Ty_ty Ty_ULONG(void);//todo qual
Ty_ty Ty_SLLONG(void);//todo qual
Ty_ty Ty_ULLONG(void);//todo qual
Ty_ty Ty_FLOAT(void);//todo qual
Ty_ty Ty_DOUBLE(void);//todo qual
Ty_ty Ty_LDOUBLE(void);//todo qual
*/

Ty_ty Ty_Name(S_symbol sym, Ty_ty ty);
Ty_ty Ty_Array(Ty_ty ty);
Ty_ty Ty_Array(Ty_ty ty, int num);
Ty_ty Ty_Pointer(Ty_ty ty);
Ty_ty Ty_Structty(Ty_fields fields);

//?
Ty_field Ty_Field(S_symbol name, Ty_ty ty, Ty_field next);//todo qual bit

Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail);
Ty_tyList Ty_TyList(Ty_ty head, Ty_tyList tail);
#endif