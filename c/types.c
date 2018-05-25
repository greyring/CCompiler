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
//future double unsigned signed long ...