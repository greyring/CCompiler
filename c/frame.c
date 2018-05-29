#include "util.h"
#include "frame.h"
#include "temp.h"
#include "tree.h"

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
    F_accessList locals;
    Temp_label   name;
    int          frame_size;
};

static F_access InFrame(int offset)
{
    F_access p = checked_malloc(sizeof(*p));
    p->kind = F_inFrame;
    p->u.offset = offset;
    return p;
}

static F_access InReg(Temp_temp reg)
{
    F_access p = checked_malloc(sizeof(*p));
    p->kind = F_inReg;
    p->u.reg = reg;
    return p;
}

F_accessList F_AccessList(F_access head, F_accessList tail)
{
    F_accessList p = checked_malloc(sizeof(*p));
    p->head = head; p->tail = tail;
    return p;
}

F_escapeList F_EscapeList(int escape, F_escapeList next)
{
    F_escapeList p = checked_malloc(sizeof(*p));
    p->escape = escape;
    p->next = next;
    return p;
}

F_access F_allocLocal(F_frame f, int escape)
{
    if (escape)
    {
        F_access access;
        access = InFrame(f->frame_size);
        f->frame_size += 4;
        f->locals = F_AccessList(access, f->locals);
        return access;
    }
    else
        return InReg(Temp_newtemp());
}

F_frame F_newFrame(Temp_label name, F_escapeList formals)
{

}



