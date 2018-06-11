#ifndef _FRAME_H_
#define _FRAME_H_

#include "temp.h"
#include "util.h"

typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;
typedef struct F_accessList_ *F_accessList;
typedef struct F_escapeList_ *F_escapeList;

struct F_accessList_ {F_access head; F_accessList tail;};
struct F_escapeList_ {int escape; F_escapeList next;};

F_frame F_newFrame(Temp_label name, F_escapeList formals);
F_accessList F_AccessList(F_access head, F_accessList tail);
F_escapeList F_EscapeList(int escape, F_escapeList next);
F_access F_allocLocal(F_frame f, int escape, int size);
F_access F_allocStr(F_frame f, string s);

#endif