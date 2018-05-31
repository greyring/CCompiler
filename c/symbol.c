#include <stdio.h>
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "table.h"

struct S_symbol_ {string name; S_symbol next;};
static struct S_symbol_ marksym = {"<mark>", 0};

static S_symbol mksymbol(string name, S_symbol next){
    S_symbol s = checked_malloc(sizeof(*s));
    s->name = name; s->next = next;
    return s;
}

#define SIZE 109
static S_symbol hashtable[SIZE];

static hashValue hashnum(string s){
    hashValue num = 0;
    while(*s){
        num = (num * 256 + (hashValue)*s) % SIZE;
    }
    return num;
}

string S_name(S_symbol sym){
    return sym->name;
}

S_symbol S_Symbol(string name){
    hashValue value = hashnum(name);
    for (S_symbol sym=hashtable[value]; sym!=NULL; sym=sym->next){
        if (0 == strcmp(sym->name, name)){
            return sym;
        }
    }
    return NULL;
}

S_table S_empty(void){
    return TAB_empty();
}

void S_enter(S_table t, S_symbol sym, void* value){
    TAB_enter(t, (void*)sym, value);
}

void* S_look(S_table t, S_symbol sym){
    return TAB_look(t, (void*)sym);
}

void S_beginScope(S_table t){
    S_enter(t, &marksym, NULL);
}

void S_endScope(S-table t){
    S_Symbol s;
    do{
        s = TAB_pop(t);
    }while (s != &marksym)

}