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

// 一个类型
struct Ty_ty_
{
    //Ty_qual_ qual;
    // 所有type的定义，太多了
    enum{
        /*
        Ty_basic_ty,
        Ty_name_ty,
        Ty_array_ty,
        Ty_func_ty,
        Ty_structty_ty,
        Ty_pointer_ty,
        */

        //以下是specifier
        Ty_spec_ty,
            Ty_specifier_ty, //包括以下类型
                Ty_storage_ty,//以下是storage type的类型
                    Ty_typedef_ty, 
                    Ty_extern_ty,
                    Ty_static_ty,
                    Ty_auto_ty,
                    Ty_register_ty,
                /*Ty_type_ty, *///type下文再描述
                Ty_qualifier_ty, //以下是qualifier的类型
                    Ty_const_ty,
                    Ty_restrict_ty,
                    Ty_volatile_ty,
                Ty_func_ty, //以下是function的类型
                    Ty_inline_ty,
                Ty_spec_seq_ty, //这是seq的类型

        //以下是A_type
        Ty_type_ty,
            Ty_primtype_ty, //以下是primary type的类型，这都是基本类型，参数使用void
                Ty_void_ty,
                Ty_char_ty,
                Ty_short_ty,
                Ty_int_ty,
                Ty_long_ty,
                Ty_float_ty,
                Ty_double_ty,
                Ty_signed_ty,
                Ty_unsigned_ty,
                Ty_struct_ty,
                Ty_union_ty,
                Ty_enum_ty,    
            Ty_typeid_ty, //这是typeid，这里有点问题，下面写函数的时候再考虑
            Ty_struct_union_ty, //这是struct or union类型，包括以下类型
                /*Ty_primtype_ty*/ //上文有了
                /*Ty_typeid_ty*/ //上文有了
                /*Ty_declaration_ty*/ // 下文再描述
            Ty_enumtype_ty,
                /*Ty_typeid_ty*/ //上文有了
                /*Ty_init_ty*/ //下文再描述
        
        //以下是A_pointer类型
        Ty_pointer_ty,
            /*Ty_spec_ty*/ //上文有了
            Ty_pointer_seq_ty,
        
        //以下是A_param类型
        Ty_param_ty,
            /*Ty_param_ty*/
            Ty_param_seq_ty,
            Ty_param_dec_ty,
                /*Ty_spec_ty,*/
                /*Ty_dec_ty*/
        
        //以下是A_dec类型
        Ty_dec_ty,
            Ty_simple_ty, //这有点问题
            Ty_dec_seq_ty,
            Ty_subinit_ty,
                /*Ty_dec_ty,*/
                /*Ty_init_ty*/ //下文再描述
            Ty_subbit_ty,
                /*Ty_exp_ty*/ //这里有点问题
            Ty_subpointer_ty,
            Ty_subarray_ty,


        
        //Ty_unionty_ty,
        //enum is a list of constant int defination
        //enum itself is an int
    }kind; //kind应该包括所有数据类型
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
        Ty_field structty_ty;
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

Ty_ty Ty_VOID(void); //基本类型的绑定，参数是void
Ty_ty Ty_INT(void);  //基本类型的绑定，参数是void
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

// 复杂类型的包装，把基本类型的Ty_ty二次包装成Ty_ty
Ty_ty Ty_Name(S_symbol sym, Ty_ty ty);
Ty_ty Ty_Array(Ty_ty ty);
Ty_ty Ty_Array(Ty_ty ty, int num);
Ty_ty Ty_Pointer(Ty_ty ty);
Ty_ty Ty_Structty(Ty_field fields);

//一个包装好的binding，把symbol和Ty_ty绑定成一个Ty_field
Ty_field Ty_Field(S_symbol name, Ty_ty ty, Ty_field next);//todo qual bit

Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail);
Ty_tyList Ty_TyList(Ty_ty head, Ty_tyList tail);
#endif