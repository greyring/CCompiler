/*
 * temp.c - functions to create and manipulate temporary variables which are
 *          used in the IR tree representation before it has been determined
 *          which variables are to go into registers.
 *
 */
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "table.h"
#include <assert.h>
#include <stdio.h>

static int labels = 0;
static int temps = 100;

Temp_label Temp_newlabel(void)
{
    char buf[100];
    sprintf(buf,"<L%d",labels++);
    return Temp_namedlabel(String(buf));
}

/* The label will be created only if it is not found. */
Temp_label Temp_namedlabel(string s)
{
    return _S_symbol(s);
}

Temp_temp Temp_newtemp(void)
{
    Temp_temp p = checked_malloc(sizeof (*p));
    p->num=temps++;
    {
        char r[16];
        sprintf(r, "%d", p->num);
        Temp_enter(Temp_name(), p, String(r));
    }
    return p;
}

Temp_tempList Temp_TempList(Temp_temp h, Temp_tempList t) 
{
    Temp_tempList p = (Temp_tempList) checked_malloc(sizeof (*p));
    p->head=h; p->tail=t;
    return p;
}

Temp_labelList Temp_LabelList(Temp_label h, Temp_labelList t)
{
    Temp_labelList p = (Temp_labelList) checked_malloc(sizeof (*p));
    p->head=h; p->tail=t;
    return p;
}




static Temp_map newMap(TAB_table tab, Temp_map under) 
{
    Temp_map m = checked_malloc(sizeof(*m));
    m->tab=tab;
    m->under=under;
    return m;
}

Temp_map Temp_empty(void) {
    return newMap(TAB_empty(), NULL);
}

//the outer most temp map
Temp_map Temp_name(void) {
    static Temp_map m = NULL;
    if (!m) m=Temp_empty();
    return m;
}

Temp_map Temp_layerMap(Temp_map over, Temp_map under) 
{
    if (over==NULL)
        return under;
    else return newMap(over->tab, Temp_layerMap(over->under, under));
}

void Temp_enter(Temp_map m, Temp_temp t, string s) 
{
    assert(m && m->tab);
    TAB_enter(m->tab,t,s);
}

string Temp_look(Temp_map m, Temp_temp t) 
{
    string s;
    assert(m && m->tab);
    s = TAB_look(m->tab, t);
    if (s) return s;
    else if (m->under) return Temp_look(m->under, t);
    else return NULL;
}
