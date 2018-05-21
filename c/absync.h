#ifndef _ABSYNC_H_
#define _ABSYNC_H_

#include "symbol.h"

typedef int A_pos;

typedef struct A_exp_ *A_exp;
typedef struct A_spec_ *A_spec;
typedef struct A_type_ *A_type;
typedef struct A_pointer_ *A_pointer;
typedef struct A_param_ *A_param;
typedef struct A_dec_ *A_dec;//declator
typedef struct A_declaration_ *A_declaration;
typedef struct A_type_name_ *A_type_name;
typedef struct A_designator_ *A_designator;
typedef struct A_init_ *A_init;
typedef struct A_stat_ *A_stat; 
typedef struct A_def_  *A_def;

typedef enum{
    A_AND, A_NOT, A_XOR, A_OR, A_LSHIFT, A_RSHIFT,
    A_DIV, A_MOD, A_MUL, A_PLUS, A_MINUS,
    A_LT, A_GT, A_LTE, A_GTE, A_EQ, A_NEQ,
    A_LOGNOT, A_LOGAND, A_LOGOR,
    A_THREE,
    A_ASSIGN, A_MULA, A_DIVA, A_MODA,
    A_PLUSA, A_MINUSA, A_LSHIFTA, A_RSHIFTA,
    A_ANDA, A_XORA, A_ORA 
}A_op;

typedef enum{
    A_TYPEDEF, A_EXTERN, A_STATIC, A_AUTO, A_REGISTER
}A_storage_type;

typedef enum{
    A_VOID, A_CHAR, A_SHORT, A_INT, A_LONG, A_FLOAT,
    A_DOUBLE, A_SIGNED, A_UNSIGNED,
    A_STRUCT, A_UNION, A_ENUM
}A_prim_type;

typedef enum{
    A_CONST, A_RESTRICT, A_VOLATILE
}A_qual_type;

typedef enum{
    A_INLINE
}A_func_type;

struct A_exp_
{
    A_pos pos;
    enum{
        //postfix
        A_id_exp,
        A_intexp_exp,
        A_floatexp_exp,
        A_charexp_exp,
        A_strexp_exp,
        A_subscript_exp,
        A_funccall_exp,
        A_dot_exp,
        A_point_exp,
        A_postpp_exp,
        A_postmm_exp,
        A_init_exp,
        //unary
        A_prepp_exp,
        A_premm_exp,
        A_unaryop_exp,
        A_sizeof_unary_exp,
        A_sizeof_type_exp,
        //cast
        A_cast_exp,
        //op
        A_binop_exp,
        A_threeop_exp,
        //assign
        A_assign_exp,
        A_seq_exp
    }kind;
    union{
        S_symbol id;
        int intexp;
        double floatexp;
        char charexp;
        char* strexp;
        struct{
            A_exp expr;
            A_exp subscript;
        }subscript;
        struct{
            A_exp expr;
            A_exp args;
        }funccall;
        struct{
            A_exp expr;
            S_symbol id;
        }dot, point;
        A_exp postpp, postmm, prepp, premm, sizeof_unary;
        struct{
            A_type_name type_name;
            A_init init_list;
        }init;
        struct{
           A_op op;
           A_exp expr;
        }unaryop;
        A_type_name sizeof_type;
        struct{
            A_type_name type;
            A_exp expr;
        }cast;
        struct{
            A_op op;
            A_exp expr1;
            A_exp expr2;
        }binop;
        struct{
            A_exp cond;
            A_exp expr1;
            A_exp expr2;
        }threeop;
        struct{
            A_exp dst;
            A_op  op;
            A_exp src;
        }assign;
        struct{
            A_exp expr;
            A_exp next;
        }seq;
    }u;
};

struct A_spec_
{
    A_pos pos;
    enum{
        A_storage_spec,
        A_type_spec,
        A_qual_type_spec,
        A_func_type_spec,
        A_seq_spec
    }kind;
    union{
        A_storage_type storage;
        A_type type;
        A_qual_type qual_type;
        A_func_type func_type;
        struct{
            A_spec spec;
            A_spec next;
        }seq;
    }u;
};

struct A_type_
{
    A_pos pos;
    enum{
        A_simple_type,
        A_struct_union_type,
        A_enumtype_type,
        A_typeid_type
    }kind;
    union{
        A_prim_type simple;
        S_symbol typeid;
        struct{
            A_prim_type struct_union;
            S_symbol id;
            A_declaration dec_list;
        }struct_union;
        struct{
            S_symbol id;
            A_init init_list;
        }enumtype;
    }u;
};

struct A_pointer_
{
    A_pos pos;
    enum{
        A_simple_pointer,
        A_seq_pointer
    }kind;
    union{
        A_spec simple;//qual_list
        struct{
            A_pointer pointer;
            A_pointer next;
        }seq;
    }u;
};

struct A_param_
{
    A_pos pos;
    enum{
        A_uncertain_param,
        A_seq_param,
        A_dec_param
    }kind;
    union{
        A_param uncertain;
        struct{
            A_param param;
            A_param next;
        }seq;
        struct{
            A_spec spec;
            A_dec  dec;
        }dec;
    }u;
};

struct A_dec_
{
    A_pos pos;
    enum{
        A_simple_dec,
        A_seq_dec,
        A_init_dec,
        A_bit_dec,
        A_pointer_dec,
        A_array_dec,
        A_array_proto_dec,
        A_func_dec,
        A_funcid_dec,
    }kind;
    union{
        S_symbol simple;
        struct{
            A_dec dec;
            A_dec next;
        }seq;
        struct{
            A_dec dec;
            A_init init;
        }init;
        struct{
            A_dec dec;
            A_exp const_exp;
        }bit;
        struct{
            A_pointer pointer;
            A_dec dec;
        }pointer;
        struct{
            A_dec dec;
            A_exp assign_exp;
        }array;
        A_dec array_proto;
        struct{
            A_dec dec;
            A_param param_list;
        }func;
        struct{
            A_dec dec;
            A_exp id_list;
        }funcid;
    }u;
};

struct A_declaration_
{
    A_pos pos;
    enum{
        A_simple_declaration,
        A_seq_declaration
    }kind;
    union{
        struct{
            A_spec spec;
            A_dec  dec;
        }simple;
        struct{
            A_declaration declaration;
            A_declaration next;
        }seq;
    }u;
};

struct A_type_name_
{
    A_pos pos;
    enum{
        A_simple_type_name
    }kind;
    union{
        struct{
            A_spec spec;
            A_dec  abs_dec;
        }simple;
    }u;
};

struct A_designator_
{
    A_pos pos;
    enum{
        A_seq_designator,
        A_subscript_designator,
        A_dot_designator
    }kind;
    union{
        struct{
            A_designator designator;
            A_designator next;
        }seq;
        A_exp subscript;//const_exp
        S_symbol dot;//id
    }u;
};

struct A_init_
{
    A_pos pos;
    enum{
        A_simple_init,
        A_seq_init,
        A_enumtype_init,
        A_designation_init,
    }kind;
    union
    {
        A_exp simple;//assign_exp
        struct{
            A_init init;
            A_init next;
        }seq;
        struct{
            S_symbol id;
            A_exp const_exp;
        }enumtype;
        struct{
            A_designator designator;
            A_init init;
        }designation;
    }u;
};

struct A_stat_
{
    A_pos pos;
    enum{
        A_exp_stat,
        A_dec_stat,
        A_seq_stat,
        A_label_stat,
        A_casestat_stat,
        A_defaultstat_stat,
        A_ifstat_stat,
        A_switchstat_stat,
        A_whilestat_stat,
        A_dowhile_stat,
        A_forexp_stat,
        A_fordec_stat,
        A_gotostat_stat,
        A_continuestat_stat,
        A_breakstat_stat,
        A_returnstat_stat
    }kind;
    union{
        A_exp exp;
        A_declaration dec;
        struct{
            A_stat stat;
            A_stat next;
        }seq;
        struct{
            S_symbol id;
            A_stat stat;
        }label;
        struct{
            A_exp const_exp;
            A_stat stat;
        }casestat;
        A_stat defaultstat;
        struct{
            A_exp exp;
            A_stat stat_true;
            A_stat  stat_false;
        }ifstat;
        struct{
            A_exp exp;
            A_stat stat;
        }switchstat, whilestat, dowhile;
        struct{
            A_exp exp_1, exp_2, exp_3;
            A_stat stat;
        }forexp;
        struct{
            A_declaration dec;
            A_exp exp_2, exp_3;
            A_stat stat;
        }fordec;
        S_symbol gotostat;
        A_exp returnstat;
    }u;
};

struct A_def_
{
    A_pos pos;
    enum{
        A_simple_def,
        A_seq_def,
        A_func_def
    }kind;
    union{
        struct{
            A_def def;
            A_def next;
        }seq;
        A_declaration simple;
        struct{
            A_spec spec;
            A_dec func;
            A_declaration args;
            A_stat stat;
        }func;
    }u;
};

A_exp _A_id_exp(A_pos, S_symbol);
A_exp _A_intexp_exp(A_pos, int);
A_exp _A_floatexp_exp(A_pos, double);
A_exp _A_charexp_exp(A_pos, char);
A_exp _A_strexp_exp(A_pos, char*);
A_exp _A_subscript_exp(A_pos, A_exp, A_exp);
A_exp _A_funccall_exp(A_pos, A_exp, A_exp);
A_exp _A_dot_exp(A_pos, A_exp, S_symbol);
A_exp _A_point_exp(A_pos, A_exp, S_symbol);
A_exp _A_postpp_exp(A_pos, A_exp);
A_exp _A_postmm_exp(A_pos, A_exp);
A_exp _A_init_exp(A_pos, A_type_name, A_init);
A_exp _A_prepp_exp(A_pos, A_exp);
A_exp _A_premm_exp(A_pos, A_exp);
A_exp _A_unaryop_exp(A_pos, A_op, A_exp);
A_exp _A_sizeof_unary_exp(A_pos, A_exp);
A_exp _A_sizeof_type_exp(A_pos, A_type_name);
A_exp _A_cast_exp(A_pos, A_type_name, A_exp);
A_exp _A_binop_exp(A_pos, A_op, A_exp, A_exp);
A_exp _A_threeop_exp(A_pos, A_exp, A_exp, A_exp);
A_exp _A_assign_exp(A_pos, A_exp, A_op, A_exp);
A_exp _A_seq_exp(A_pos, A_exp, A_exp);

A_type _A_simple_type(A_pos, A_prim_type);
A_type _A_typeid_type(A_pos, S_symbol);
A_type _A_struct_union_type(A_pos, A_prim_type, S_symbol, A_declaration);
A_type _A_enumtype_type(A_pos, S_symbol, A_init);

A_spec _A_storage_spec(A_pos, A_storage_type);
A_spec _A_type_spec(A_pos, A_type);
A_spec _A_qual_type_spec(A_pos, A_qual_type);
A_spec _A_func_type_spec(A_pos, A_func_type);
A_spec _A_seq_spec(A_pos, A_spec, A_spec);

A_pointer _A_simple_pointer(A_pos, A_spec);
A_pointer _A_seq_pointer(A_pos, A_pointer, A_pointer);

A_param _A_uncertain_param(A_pos, A_param);
A_param _A_seq_param(A_pos, A_param, A_param);
A_param _A_dec_param(A_pos, A_spec, A_dec);

A_dec _A_simple_dec(A_pos, S_symbol);
A_dec _A_seq_dec(A_pos, A_dec, A_dec);
A_dec _A_init_dec(A_pos, A_dec, A_init);
A_dec _A_bit_dec(A_pos, A_dec, A_exp);
A_dec _A_pointer_dec(A_pos, A_pointer, A_dec);
A_dec _A_array_dec(A_pos, A_dec, A_exp);
A_dec _A_array_proto_dec(A_pos, A_dec);
A_dec _A_func_dec(A_pos, A_dec, A_param);
A_dec _A_funcid_dec(A_pos, A_dec, A_exp);

A_declaration _A_simple_declaration(A_pos, A_spec, A_dec);
A_declaration _A_seq_declaration(A_pos, A_declaration, A_declaration);

A_type_name _A_simple_type_name(A_pos, A_spec, A_dec);

A_designator _A_seq_designator(A_pos, A_designator, A_designator);
A_designator _A_subscript_designator(A_pos, A_exp);
A_designator _A_dot_designator(A_pos, S_symbol);

A_init _A_simple_init(A_pos, A_exp);
A_init _A_seq_init(A_pos, A_init, A_init);
A_init _A_enumtype_init(A_pos, S_symbol, A_exp);
A_init _A_designation_init(A_pos, A_designator, A_init);


A_stat _A_exp_stat(A_pos, A_exp);
A_stat _A_dec_stat(A_pos, A_declaration);
A_stat _A_seq_stat(A_pos, A_stat, A_stat);
A_stat _A_label_stat(A_pos, S_symbol, A_stat);
A_stat _A_casestat_stat(A_pos, A_exp, A_stat);
A_stat _A_defaultstat_stat(A_pos, A_stat);
A_stat _A_ifstat_stat(A_pos, A_exp, A_stat, A_stat);
A_stat _A_switchstat_stat(A_pos, A_exp, A_stat);
A_stat _A_whilestat_stat(A_pos, A_exp, A_stat);
A_stat _A_dowhile_stat(A_pos, A_stat, A_exp);
A_stat _A_forexp_stat(A_pos, A_exp, A_exp, A_exp, A_stat);
A_stat _A_fordec_stat(A_pos, A_declaration, A_exp, A_exp, A_stat);
A_stat _A_gotostat_stat(A_pos, S_symbol);
A_stat _A_continuestat_stat(A_pos);
A_stat _A_breakstat_stat(A_pos);
A_stat _A_returnstat_stat(A_pos, A_exp);

A_def _A_simple_def(A_pos, A_declaration);
A_def _A_seq_def(A_pos, A_def, A_def);
A_def _A_func_def(A_pos, A_spec, A_dec, A_declaration, A_stat);

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

#endif

