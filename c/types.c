#include "types.h"

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
BASIC(INT);

Ty_ty Ty_Name(S_symbol sym, Ty_ty ty){
    Ty_ty_ t;
    t.kind = Ty_name_ty;
    t.u.name.name = sym;
    t.u.name.Ty_ty = ty;
    return &t;
}

Ty_ty Ty_Array(Ty_ty ty){
    Ty_ty_ t;
    t.kind = Ty_array_ty;
    t.u.array.array = ty;
    t.u.array.num = -1;
    return &t;
}

Ty_ty Ty_Array(Ty_ty ty, int num){
    Ty_ty_ t;
    t.kind = Ty_array_ty;
    t.u.array.array = ty;
    t.u.array.num = num;
    return &t;
}

Ty_ty Ty_Pointer(Ty_ty ty){
    Ty_ty_ t;
    t.kind = Ty_pointer_ty;
    t.u.pointer = ty;
    return &t;
}

Ty_ty Ty_Structty(Ty_fields fields){
    Ty_ty_ t;
    t.kind = Ty_structty_ty;
    t.u.structty_ty = fields;
    return &t;
}

//?
Ty_field Ty_Fields(S_symbol name, Ty_ty ty, Ty_field next){
    Ty_field_ t;
    t.type = ty;
    t.name = name;
    return &t;
}

Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail){
    Ty_fieldList_ t;
    t.head = head;
    t.tail = tail;
    return &t;
};

Ty_tyList Ty_TyList(Ty_ty head, Ty_tyList tail){
    Ty_tyList_ t;
    t.head = head;
    t.tail = tail;
    return &t;
}
//future double unsigned signed long ...