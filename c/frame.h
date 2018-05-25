#ifndef _FRAME_H_
#define _FRAME_H_

typedef struct F_frag_ *F_frag;
typedef struct F_fragList_ *F_fragList;
typedef struct F_frame_ *F_frame;
typedef struct F_access_ *F_access;
typedef struct F_accessList_ *F_accessList;
typedef struct F_escapeList_ *F_escapeList;




struct F_fragList_ {F_fragList head, F_fragList frags};
struct F_accessList_ {F_access head; F_accessList tail;};
struct F_escapeList_ {int escape; F_escapeList next;};

F_fragList F_FragList(F_fragList head, F_fragList frags);
F_frame F_newFrame(Temp_label name, F_escapeList formals);
F_escapeList F_EscapeList(int escape, F_escapeList next);

#endif