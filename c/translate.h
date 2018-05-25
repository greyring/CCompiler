#ifndef _TRANSLATE_H_
#define _TRANSLATE_H_

#include "frame.h"

typedef struct Tr_level_ *Tr_level;

struct Tr_level_
{
    Tr_level parent;
    F_frame frame;  
};

#endif