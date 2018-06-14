#ifndef _FRAME_H_
#define _FRAME_H_

#include "temp.h"
#include "util.h"
#include "tree.h"
typedef struct Ty_fieldList_ Ty_fieldList;//a struct

typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;
typedef struct F_accessList_ *F_accessList;
typedef struct F_escapeList_ *F_escapeList;
typedef struct F_frag_ *F_frag;

struct F_accessList_ {F_access head; F_accessList tail;};
struct F_escapeList_ {int escape; F_escapeList next;};

struct F_frag_ {
    T_stm body;
    F_frame frame;
};

Temp_temp F_FP(void);

Temp_temp F_SP(void);

Temp_temp F_RA(void);

Temp_temp F_V0(void);

F_frame F_newFrame(Temp_label name, Ty_fieldList formals);
F_accessList F_AccessList(F_access head, F_accessList tail);
F_escapeList F_EscapeList(int escape, F_escapeList next);
F_access F_allocLocal(F_frame f, int escape, int size);
F_access F_allocStr(F_frame f, string s);

T_exp F_Var(F_access access, T_exp framePtr);
T_exp F_ExVar(S_symbol sym);


#endif