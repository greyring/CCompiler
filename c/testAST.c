#include "absync.h"
#include "testAST.h"
#include "util.h"
#include "symbol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define dinc if (!in)\
        {\
            inc("NULL");\
            return ;\
        }\
        depth++ \
        
#define ddec depth--

#define TA_n1(name, type, atype) \
        case A_##name##_##type: \
            inc("A_"#name"_"#type""); \
            TA_##atype(in->u.name); \
            break

#define TA_1(name, type, atype, a) \
        case A_##name##_##type: \
            inc("A_"#name"_"#type""); \
            TA_##atype(in->u.name.a); \
            break

#define TA_2(name, type, atype, a, btype, b) \
        case A_##name##_##type: \
            inc("A_"#name"_"#type""); \
            TA_##atype(in->u.name.a); \
            TA_##btype(in->u.name.b); \
            break

#define TA_3(name, type, atype, a, btype, b, ctype, c) \
        case A_##name##_##type: \
            inc("A_"#name"_"#type""); \
            TA_##atype(in->u.name.a); \
            TA_##btype(in->u.name.b); \
            TA_##ctype(in->u.name.c); \
            break

#define TA_4(name, type, atype, a, btype, b, ctype, c, dtype, d) \
        case A_##name##_##type: \
            inc("A_"#name"_"#type""); \
            TA_##atype(in->u.name.a); \
            TA_##btype(in->u.name.b); \
            TA_##ctype(in->u.name.c); \
            TA_##dtype(in->u.name.d); \
            break

#define TA_5(name, type, atype, a, btype, b, ctype, c, dtype, d, etype, e) \
        case A_##name##_##type: \
            inc("A_"#name"_"#type""); \
            TA_##atype(in->u.name.a); \
            TA_##btype(in->u.name.b); \
            TA_##ctype(in->u.name.c); \
            TA_##dtype(in->u.name.d); \
            TA_##etype(in->u.name.e); \
            break

int depth = 0;


struct Log{
    char *str;
    int length;
    int nlen;
}llog;
void clearLog(){
    if (llog.str == NULL) free(llog.str);
    llog.length = 1000;
    llog.str = (char*)checked_malloc(sizeof(char) * (llog.length + 1));
    llog.str[0] = '\0';
    llog.nlen = 0;
}

char* gstr(char* a){
    char* b = (char*)checked_malloc(sizeof(char) * (strlen(a) + depth + 5));
    for (int i=0; i<depth-1; i++){
        b[i] = '\t';
        b[i+1] = '\0';
    }
    strcat(b, a);
    strcat(b, "\n");
    return b;
}

void inc(char* a){
    char* b = gstr(a);
    int len = strlen(b);
    if (llog.nlen + len >= llog.length){
        char* nstr = (char*)checked_malloc(sizeof(char) * (llog.length + len + 1000 +1));
        if (llog.str)
            strcpy(nstr, llog.str);
        llog.length = llog.length + len + 1000;
        free(llog.str); llog.str = nstr;
    }
    strcat(llog.str, b);
    llog.nlen = strlen(llog.str);
    free(b);
}

char *TA_getRes(){
    return llog.str;
}

char *TA_op_array[32] = {"A_AND", "A_NOT", "A_XOR", "A_OR", "A_LSHIFT", "A_RSHIFT",
    "A_DIV", "A_MOD", "A_MUL", "A_PLUS", "A_MINUS",
    "A_LT", "A_GT", "A_LTE", "A_GTE", "A_EQ", "A_NEQ",
    "A_LOGNOT", "A_LOGAND", "A_LOGOR",
    "A_THREE",
    "A_ASSIGN", "A_MULA", "A_DIVA", "A_MODA",
    "A_PLUSA", "A_MINUSA", "A_LSHIFTA", "A_RSHIFTA",
    "A_ANDA", "A_XORA", "A_ORA"};

char *TA_storage_type_array[5] = {"A_TYPEDEF", "A_EXTERN", "A_STATIC", "A_AUTO", "A_REGISTER"};

char *TA_prim_type_array[12] = {"A_VOID", "A_CHAR", "A_SHORT", "A_INT", "A_LONG", "A_FLOAT",
    "A_DOUBLE", "A_SIGNED", "A_UNSIGNED",
    "A_STRUCT", "A_UNION", "A_ENUM"};

char *TA_qual_type_array[3] = {"A_CONST", "A_RESTRICT", "A_VOLATILE"};

char *TA_func_type_array[1] = {"A_INLINE"};

void TA_exp(A_exp in){
    dinc;
    char *ss = (char*)checked_malloc(sizeof(char) * 100);
    switch(in->kind){
        case A_intexp_exp:
            sprintf(ss, "A_intexp_exp : %d", in->u.intexp);
            inc(ss); free(ss);
            break;
        case A_floatexp_exp:
            sprintf(ss, "A_floatexp_exp : %lf", in->u.floatexp);
            inc(ss); free(ss);
            break;
        case A_charexp_exp:
            sprintf(ss, "A_charexp_exp : %c", in->u.charexp);
            inc(ss); free(ss);
            break;
        case A_strexp_exp:
            sprintf(ss, "A_strexp_exp : %s", in->u.strexp);
            inc(ss);
            break;
        TA_n1(id, exp, symbol);
        TA_2(subscript, exp, exp, expr, exp, subscript);
        TA_2(funccall, exp, exp, expr, exp, args);
        TA_2(dot, exp, exp, expr, symbol, id);
        TA_2(point, exp, exp, expr, symbol, id);
        TA_n1(postpp, exp, exp);
        TA_n1(postmm, exp, exp);
        TA_n1(prepp, exp, exp);
        TA_n1(premm, exp, exp);
        TA_n1(sizeof_unary, exp, exp);
        TA_2(init, exp, type_name, type_name, init, init_list);
        TA_2(unaryop, exp, op, op, exp, expr);
        TA_n1(sizeof_type, exp, type_name);
        TA_2(cast, exp, type_name, type, exp, expr);
        TA_3(binop, exp, op, op, exp, expr1, exp, expr2);
        TA_3(threeop, exp, exp, cond, exp, expr1, exp, expr2);
        TA_3(assign, exp, exp, dst, op, op, exp, src);
        TA_2(seq, exp, exp, expr, exp, next);
    }
    ddec;
}

void TA_spec(A_spec in){
    dinc;
    switch (in->kind){
        TA_n1(storage, spec, storage_type);
        TA_n1(type, spec, type);
        TA_n1(qual_type, spec, qual_type);
        TA_n1(func_type, spec, func_type);
        TA_2(seq, spec, spec, spec, spec, next);
    }
    ddec;
}

void TA_type(A_type in){
    dinc;
    switch(in->kind){
        TA_n1(simple, type, prim_type);
        TA_3(struct_union, type, prim_type, struct_union, symbol, id, declaration, dec_list);
        TA_2(enumtype, type, symbol, id, init, init_list);
        TA_n1(typeid, type, symbol);
    }
    ddec;
}

void TA_pointer(A_pointer in){
    dinc;
    switch(in->kind){
        TA_n1(simple, pointer, spec);
        TA_2(seq, pointer, pointer, pointer, pointer, next);
    }
    ddec;
}

void TA_param(A_param in){
    dinc;
    switch(in->kind){
        TA_n1(uncertain, param, param);
        TA_2(seq, param, param, param, param, next);
        TA_2(dec, param, spec, spec, dec, dec);
    }
    ddec;
}

void TA_dec(A_dec in){
    dinc;
    switch(in->kind){
        TA_n1(simple, dec, symbol);
        TA_2(seq, dec, dec, dec, dec, next);
        TA_2(init, dec, dec, dec, init, init);
        TA_2(bit, dec, dec, dec, exp, const_exp);
        TA_2(pointer, dec, pointer, pointer, dec, dec);
        TA_2(array, dec, dec, dec, exp, assign_exp);
        TA_n1(array_proto, dec, dec);
        TA_2(func, dec, dec, dec, param, param_list);
        TA_2(funcid, dec, dec, dec, exp, id_list);
    }
    ddec;
}

void TA_declaration(A_declaration in){
    dinc;
    switch(in->kind){
        TA_2(simple, declaration, spec, spec, dec, dec);
        TA_2(seq, declaration, declaration, declaration, declaration, next);
    }
    ddec;
}

void TA_type_name(A_type_name in){
    dinc;
    switch(in->kind){
        TA_2(simple, type_name, spec, spec, dec, abs_dec);
    }
    ddec;
}

void TA_designator(A_designator in){
    dinc;
    switch(in->kind){
        TA_n1(dot, designator, symbol);
        TA_2(seq, designator, designator, designator, designator, next);
        TA_n1(subscript, designator, exp);
    }
    ddec;
}

void TA_init(A_init in){
    dinc;
    switch(in->kind){
        TA_n1(simple, init, exp);
        TA_2(seq, init, init, init, init, next);
        TA_2(enumtype, init, symbol, id, exp, const_exp);
        TA_2(designation, init, designator, designator, init, init);
    }
    ddec;
}

void TA_stat(A_stat in){
    dinc;
    switch(in->kind){
        TA_n1(exp, stat, exp);
        TA_n1(dec, stat, declaration);
        TA_2(seq, stat, stat, stat, stat, next);
        TA_2(label, stat, stat, stat, symbol, id);
        TA_2(casestat, stat, exp, const_exp, stat, stat);
        TA_n1(defaultstat, stat, stat);
        TA_3(ifstat, stat, exp, exp, stat, stat_true, stat, stat_false);
        TA_2(switchstat, stat, exp, exp, stat, stat);
        TA_2(whilestat, stat, exp, exp, stat, stat);
        TA_2(dowhile, stat, exp, exp, stat, stat);
        TA_4(forexp, stat, exp, exp_1, exp, exp_2, exp, exp_3, stat, stat);
        TA_4(fordec, stat, declaration, dec, exp, exp_2, exp, exp_3, stat, stat);
        TA_n1(returnstat, stat, exp);
        TA_n1(gotostat, stat, symbol);
        case A_continuestat_stat: break;
        case A_breakstat_stat: break;
    }
    ddec;
}

void TA_def(A_def in){
    dinc;
    switch(in->kind){
        TA_n1(simple, def, declaration);
        TA_2(seq, def, def, def, def, next);
        TA_4(func, def, spec, spec, dec, func, declaration, args, stat, stat);
    }
    ddec;
}

void TA_op(A_op in){
    char *ss = (char*)checked_malloc(sizeof(char) * 100);
    sprintf(ss, "A_op : %s", TA_op_array[in]);
    inc(ss); free(ss);
}

void TA_storage_type(A_storage_type in){
    char *ss = (char*)checked_malloc(sizeof(char) * 100);
    sprintf(ss, "A_storage_type : %s", TA_storage_type_array[in]);
    inc(ss); free(ss);
}

void TA_prim_type(A_prim_type in){
    char *ss = (char*)checked_malloc(sizeof(char) * 100);
    sprintf(ss, "A_prim_type : %s", TA_prim_type_array[in]);
    inc(ss); free(ss);
}

void TA_qual_type(A_qual_type in){
    char *ss = (char*)checked_malloc(sizeof(char) * 100);
    sprintf(ss, "A_qual_type : %s", TA_qual_type_array[in]);
    inc(ss); free(ss);
}

void TA_func_type(A_func_type in){
    char *ss = (char*)checked_malloc(sizeof(char) * 100);
    sprintf(ss, "A_func_type : %s", TA_func_type_array[in]);
    inc(ss); free(ss);
}

void TA_symbol(S_symbol in){
    if (in == NULL) {
        inc("S_symbol : NULL");
        return;
    }
    char *ss = (char*)checked_malloc(sizeof(in->name) + 1);
    sprintf(ss, "S_symbol : %s", in->name);
    inc(ss); free(ss);
}
