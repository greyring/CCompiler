#include "util.h"
#include "frame.h"
#include "temp.h"

struct F_access_
{
    enum{
        F_inFrame,
        F_inReg
    }kind;
    union{
        int offset;
        Temp_temp reg;
    }u;
};

struct F_frame_
{

};

F_fragList F_FragList(F_fragList head, F_fragList frags)
{
    head->next = frags;
    return head;
}

F_frame F_newFrame(Temp_label name, F_escapeList formals)
{

}

F_escapeList F_EscapeList(int escape, F_escapeList next)
{
    F_escapeList p = checked_malloc(sizeof(*p));
    p->escape = escape;
    p->next = next;
    return p;
}
