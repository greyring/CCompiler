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

static struct Ty_ty_ tyvoid = {Ty_VOID};


Ty_ty Ty_Name(S_symbol sym, Ty_ty ty){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_name_ty;
    t->u.name.name = sym;
    t->u.name.type = ty;
    return t;
}

Ty_ty Ty_Array(Ty_ty ty){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_array_ty;
    t->u.array.array = ty;
    t->u.array.num = -1;//-1没用？
    return t;
}

Ty_ty Ty_Array(Ty_ty ty, int num){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_array_ty;
    t->u.array.array = ty;
    t->u.array.num = num;
    return t;
}

Ty_ty Ty_Pointer(Ty_ty ty){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_pointer_ty;
    t->u.pointer = ty;
    return &t;
}

Ty_ty Ty_Structty(Ty_field fields){
    Ty_ty t = checked_malloc(sizeof(*t));
    t->kind = Ty_structty_ty;
    t->u.structty_ty = fields;
    return t;
}

//?
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