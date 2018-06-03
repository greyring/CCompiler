#include "symbol.h"
#include "absync.h"
#include "util.h"
#include <string.h>

#define A_0(type, name) \
A_##type _A_##name##_##type (A_pos pos)\
{\
    A_##type p = checked_malloc(sizeof(*p));\
    p->kind = A_##name##_##type;\
    p->pos = pos;\
    return p;\
}

#define A_1(type, name, atype) \
A_##type _A_##name##_##type (A_pos pos, atype a)\
{\
    A_##type p = checked_malloc(sizeof(*p));\
    p->kind = A_##name##_##type;\
    p->pos = pos;\
    p->u.name = a;\
    return p;\
}

#define A_2(type, name, atype, a, btype, b) \
A_##type _A_##name##_##type (A_pos pos, atype a, btype b)\
{\
    A_##type p = checked_malloc(sizeof(*p));\
    p->kind = A_##name##_##type;\
    p->pos = pos;\
    p->u.name.a = a;\
    p->u.name.b = b;\
    return p;\
}

#define A_3(type, name, atype, a, btype, b, ctype, c) \
A_##type _A_##name##_##type (A_pos pos, atype a, btype b, ctype c)\
{\
    A_##type p = checked_malloc(sizeof(*p));\
    p->kind = A_##name##_##type;\
    p->pos = pos;\
    p->u.name.a = a;\
    p->u.name.b = b;\
    p->u.name.c = c;\
    return p;\
}

#define A_4(type, name, atype, a, btype, b, ctype, c, dtype, d) \
A_##type _A_##name##_##type (A_pos pos, atype a, btype b, ctype c, dtype d)\
{\
    A_##type p = checked_malloc(sizeof(*p));\
    p->kind = A_##name##_##type;\
    p->pos = pos;\
    p->u.name.a = a;\
    p->u.name.b = b;\
    p->u.name.c = c;\
    p->u.name.d = d;\
    return p;\
}

A_1(exp, id, S_symbol);
A_1(exp, intexp, int);
A_1(exp, floatexp, double);
A_1(exp, charexp, char);
A_1(exp, strexp, char*);
A_2(exp, subscript, A_exp, expr, A_exp, subscript);
A_2(exp, funccall, A_exp, expr, A_exp, args);
A_2(exp, dot, A_exp, expr, S_symbol, id);
A_2(exp, point, A_exp, expr, S_symbol, id);
A_1(exp, postpp, A_exp);
A_1(exp, postmm, A_exp);
A_2(exp, init, A_type_name, type_name, A_init, init_list);
A_1(exp, prepp, A_exp);
A_1(exp, premm, A_exp);
A_2(exp, unaryop, A_op, op, A_exp, expr);
A_1(exp, sizeof_unary, A_exp);
A_1(exp, sizeof_type, A_type_name);
A_2(exp, cast, A_type_name, type, A_exp, expr);
A_3(exp, binop, A_op, op, A_exp, expr1, A_exp, expr2);
A_3(exp, threeop, A_exp, cond, A_exp, expr1, A_exp, expr2);
A_3(exp, assign, A_exp, dst, A_op, op, A_exp, src);
A_2(exp, seq, A_exp, expr, A_exp, next);

A_1(type, simple, A_prim_type);
A_1(type, typeid, S_symbol);
A_3(type, struct_union, A_prim_type, struct_union, S_symbol, id, A_declaration, dec_list);
A_2(type, enumtype, S_symbol, id, A_init, init_list);

A_1(spec, storage, A_storage_type);
A_1(spec, type, A_type);
A_1(spec, qual_type, A_qual_type);
A_1(spec, func_type, A_func_type);
A_2(spec, seq, A_spec, spec, A_spec, next);

A_1(pointer, simple, A_spec);
A_2(pointer, seq, A_pointer, pointer, A_pointer, next);

A_1(param, uncertain, A_param);
A_2(param, seq, A_param, param, A_param, next);
A_2(param, dec, A_spec, spec, A_dec, dec);

A_1(dec, simple, S_symbol);
A_2(dec, seq, A_dec, dec, A_dec, next);
A_2(dec, init, A_dec, dec, A_init, init);
A_2(dec, bit, A_dec, dec, A_exp, const_exp);
A_2(dec, pointer, A_pointer, pointer, A_dec, dec);
A_2(dec, array, A_dec, dec, A_exp, assign_exp);
A_1(dec, array_proto, A_dec);
A_2(dec, func, A_dec, dec, A_param, param_list);
A_2(dec, funcid, A_dec, dec, A_exp, id_list);

A_2(declaration, simple, A_spec, spec, A_dec, dec);
A_2(declaration, seq, A_declaration, declaration, A_declaration, next);

A_2(type_name, simple, A_spec, spec, A_dec, abs_dec);

A_2(designator, seq, A_designator, designator, A_designator, next);
A_1(designator, subscript, A_exp);
A_1(designator, dot, S_symbol);

A_1(init, simple, A_exp);
A_2(init, seq, A_init, init, A_init, next);
A_2(init, enumtype, S_symbol, id, A_exp, const_exp);
A_2(init, designation, A_designator, designator, A_init, init);

A_1(stat, exp, A_exp);
A_1(stat, dec, A_declaration);
A_2(stat, seq, A_stat, stat, A_stat, next);
A_2(stat, label, S_symbol, id, A_stat, stat);
A_2(stat, casestat, A_exp, const_exp, A_stat, stat);
A_1(stat, defaultstat, A_stat);
A_3(stat, ifstat, A_exp, exp, A_stat, stat_true, A_stat, stat_false);
A_2(stat, switchstat, A_exp, exp, A_stat, stat);
A_2(stat, whilestat, A_exp, exp, A_stat, stat);
A_2(stat, dowhile, A_stat, stat, A_exp, exp);
A_4(stat, forexp, A_exp, exp_1, A_exp, exp_2, A_exp, exp_3, A_stat, stat);
A_4(stat, fordec, A_declaration, dec, A_exp, exp_2, A_exp, exp_3, A_stat, stat);
A_1(stat, gotostat, S_symbol);
A_0(stat, continuestat);
A_0(stat, breakstat);
A_1(stat, returnstat, A_exp);

A_1(def, simple, A_declaration);
A_2(def, seq, A_def, def, A_def, next);
A_4(def, func, A_spec, spec, A_dec, func, A_declaration, args, A_stat, stat);

#undef A_0
#undef A_1
#undef A_2
#undef A_3
#undef A_4