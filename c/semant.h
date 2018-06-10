#ifndef SEMANT_H
#define SEMANT_H

#include "translate.h"
#include "types.h"

typedef struct exptyNode_ *exptyNode;//arg
typedef struct exptyList_ exptyList;//args

struct expty
{
    Tr_exp exp;
    Ty_ty ty;
};

struct exptyNode_
{
    struct expty expty;
    exptyNode next;
};

struct exptyList_
{
    exptyNode head;
    exptyNode tail;
};

#endif