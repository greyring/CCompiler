#include "util.h"
#include "symbol.h"
#include "env.h"

E_envs E_base_envs(void)
{
    E_envs p = checked_malloc(sizeof(*p));
    p->tenv = S_empty(void);
    p->venv = S_empty(void);
    p->tagenv = s_empty(void);
    return p;
}