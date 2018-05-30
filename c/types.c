#include "types.h"

/*
#define BASIC(name) \
struct Ty_ty_ Ty_##name##_ = Ty_basic();\
Ty_ty Ty_##name##(void){return &Ty_##name##_;}

static Ty_ty_ Ty_basic(void)
{
    Ty_ty_ t;
    t.kind = Ty_basic_ty;
    return t;
}

BASIC(VOID);
BASIC(INT);*/



static struct Ty_ty_ tyvoid = {Ty_void};
Ty_ty Ty_Void(void){
    return &tyvoid;
}

static struct Ty_ty_ tynil = {Ty_nil};
Ty_ty Ty_Nil(void){
    return &tynil;
}

static struct Ty_ty_ typointer = {Ty_pointer};
Ty_ty Ty_Pointer(void){
    return &typointer;
}

static struct Ty_ty_ tychar = {Ty_char};
Ty_ty Ty_Char(void){
    return &tychar;
}

static struct Ty_ty_ tyshort = {Ty_short};
Ty_ty Ty_Short(void){
    return &tyshort;
}

static struct Ty_ty_ tyint = {Ty_int};
Ty_ty Ty_Int(void){
    return &tyint;
}

static struct Ty_ty_ tylong = {Ty_long};
Ty_ty Ty_Long(void){
    return &tylong;
}

static struct Ty_ty_ tyfloat = {Ty_float};
Ty_ty Ty_Float(void){
    return &tyfloat;
}

static struct Ty_ty_ tydouble = {Ty_double};
Ty_ty Ty_Int(void){
    return &tyint;
}

static struct Ty_ty_ tysigned = {Ty_signed};
Ty_ty Ty_Signed(void){
    return &tysigned;
}

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
    t->kind = Ty_Decpointer;
    t->u.decpointer.type = type;
    t->u.decpointer.pointer = pointer;
}



Ty_field Ty_Field(S_symbol name, Ty_ty ty/*, Ty_field next*/){
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
//future double unsigned signed long ...