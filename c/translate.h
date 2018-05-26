#ifndef _TRANSLATE_H_
#define _TRANSLATE_H_

#include "frame.h"

//暂时使用void*替换tr_exp，以后删除这个typedef
typedef void *Tr_exp;

typedef struct Tr_level_ *Tr_level;

struct Tr_level_
{
    Tr_level parent;
    F_frame frame;  
};

#endif