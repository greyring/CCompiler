%{
#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "absync.h"
extern int charPos;
void yyerror(char *);
int yylex(void);
A_exp root;
%}

//%define parse.error verbose

%union {int ival; char cval; char* sval; double fval; A_op op; A_storage_type storage_type; A_prim_type prim_type; A_qual_type qual_type; A_func_type func_type; A_exp exp; A_spec spec; A_type type; A_pointer pointer; A_param param; A_dec dec; A_type_name type_name_; A_designator designator; A_init init; A_stat stat;}

%token KEY_AUTO KEY_BREAK KEY_CASE KEY_CHAR KEY_CONST KEY_CONTINUE KEY_DEFAULT KEY_DO KEY_DOUBLE KEY_ELSE KEY_ENUM KEY_EXTERN KEY_FLOAT KEY_FOR KEY_GOTO KEY_IF KEY_INLINE KEY_INT KEY_LONG KEY_REGISTER KEY_RESTRICT KEY_RETURN KEY_SHORT KEY_SIGNED KEY_SIZEOF KEY_STATIC KEY_STRUCT KEY_SWITCH KEY_TYPEDEF KEY_UNION KEY_UNSIGNED KEY_VOID KEY_VOLATILE KEY_WHILE KEY_UL_BOOL KEY_UL_COMPLEX KEY_UL_IMAGINARY IDENTIFIER INT_DEC_CONST INT_OCT_CONST INT_HEX_CONST FLT_DEC_CONST FLT_HEX_CONST CHAR_CONST STR_LITERAL LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE LSHIFT RSHIFT DOT ARROW DOUBLE_PLUS DOUBLE_MINUS BIT_AND BIT_NOT BIT_XOR BIT_OR OPT_TIMES OPT_PLUS OPT_MINUS OPT_DIV OPT_MOD CMP_LESS CMP_MORE CMP_LESS_EQ CMP_MORE_EQ CMP_EQ CMP_NEQ LOG_NOT LOG_AND LOG_OR QUEST_MARK COLON_MARK SEMIC_MARK ELLIP_MARK COMMA_MARK CAL_EQ CAL_TIMES_EQ CAL_DIV_EQ CAL_MOD_EQ CAL_PLUS_EQ CAL_MINUS_EQ CAL_LEFT_EQ CAL_RIGHT_EQ CAL_AND_EQ CAL_XOR_EQ CAL_OR_EQ SHARP_MARK DOUBLE_SHARP DEFAULT

%%
start:
    expression              {root = $1.exp;}
    ;
primary_expression:
      IDENTIFIER            {$$.exp = _A_id_exp(charPos, _S_symbol($1.sval));}
    | INT_DEC_CONST         {$$.exp = _A_intexp_exp(charPos, $1.ival);}
    | INT_OCT_CONST         {$$.exp = _A_intexp_exp(charPos, $1.ival);}
    | INT_HEX_CONST         {$$.exp = _A_intexp_exp(charPos, $1.ival);}
    | FLT_DEC_CONST         {$$.exp = _A_floatexp_exp(charPos, $1.fval);}
    | FLT_HEX_CONST         {$$.exp = _A_floatexp_exp(charPos, $1.fval);}
    | CHAR_CONST            {$$.exp = _A_charexp_exp(charPos, $1.fval);}
    | STR_LITERAL           {$$.exp = _A_strexp_exp(charPos, $1.sval);}
    ;

postfix_expression:
      primary_expression                                                {$$ = $1;}
    | LPAREN expression RPAREN                                          {$$ = $2;}
    | postfix_expression LBRACKET expression RBRACKET                   {$$.exp = _A_subscript_exp(charPos, $1.exp, $3.exp);}   
    | postfix_expression LPAREN expression RPAREN                       {$$.exp = _A_funccall_exp(charPos, $1.exp, $3.exp);}
    | postfix_expression LPAREN RPAREN                                  {$$.exp = _A_funccall_exp(charPos, $1.exp, NULL);}
    | postfix_expression DOT IDENTIFIER                                 {$$.exp = _A_dot_exp(charPos, $1.exp, _S_symbol($3.sval));}
    | postfix_expression ARROW IDENTIFIER                               {$$.exp = _A_point_exp(charPos, $1.exp, _S_symbol($3.sval));}
    | postfix_expression DOUBLE_PLUS                                    {$$.exp = _A_postpp_exp(charPos, $1.exp);}
    | postfix_expression DOUBLE_MINUS                                   {$$.exp = _A_postmm_exp(charPos, $1.exp);}
    | LPAREN type_name RPAREN LBRACE initializer_list RBRACE          {$$.exp = _A_init_exp(charPos, $2.type_name_, $5.init);}
    | LPAREN type_name RPAREN LBRACE initializer_list COMMA_MARK RBRACE    {$$.exp = _A_init_exp(charPos, $2.type_name_, $5.init);}
    ;

unary_expression:
      postfix_expression                {$$ = $1;}
    | DOUBLE_PLUS unary_expression      {$$.exp = _A_prepp_exp(charPos, $2.exp);}
    | DOUBLE_MINUS unary_expression     {$$.exp = _A_premm_exp(charPos, $2.exp);}
    | unary_operator cast_expression    {$$.exp = _A_unaryop_exp(charPos, $1.op, $2.exp);}
    | KEY_SIZEOF unary_expression       {$$.exp = _A_sizeof_unary_exp(charPos, $2.exp);}
    | KEY_SIZEOF LPAREN type_name RPAREN {$$.exp = _A_sizeof_type_exp(charPos, $3.type_name_);}
    ;
unary_operator:
      BIT_AND       {$$.op = A_and;}
    | OPT_TIMES     {$$.op = A_mul;}
    | OPT_PLUS      {$$.op = A_plus;}
    | OPT_MINUS     {$$.op = A_minus;}
    | BIT_NOT       {$$.op = A_not;}
    | LOG_NOT       {$$.op = A_log_not;}
    ;
cast_expression:
      unary_expression                                  {$$ = $1;}
    | LPAREN type_name RPAREN cast_expression         {$$.exp = _A_cast_exp(charPos, $2.type_name_, $4.exp);}
    ;
multiplicative_expression:
      cast_expression                                       {$$ = $1;}
    | multiplicative_expression OPT_TIMES cast_expression   {$$.exp = _A_binop_exp(charPos, A_mul, $1.exp, $3.exp);}
    | multiplicative_expression OPT_DIV cast_expression     {$$.exp = _A_binop_exp(charPos, A_div, $1.exp, $3.exp);}
    | multiplicative_expression OPT_MOD cast_expression     {$$.exp = _A_binop_exp(charPos, A_mod, $1.exp, $3.exp);}
    ;
additive_expression:
      multiplicative_expression
    | additive_expression OPT_PLUS multiplicative_expression    {$$.exp = _A_binop_exp(charPos, A_plus, $1.exp, $3.exp);}
    | additive_expression OPT_MINUS multiplicative_expression   {$$.exp = _A_binop_exp(charPos, A_minus, $1.exp, $3.exp);}
    ;
shift_expression:
      additive_expression                           {$$ = $1;}
    | shift_expression LSHIFT additive_expression   {$$.exp = _A_binop_exp(charPos, A_lshift, $1.exp, $3.exp);}
    | shift_expression RSHIFT additive_expression   {$$.exp = _A_binop_exp(charPos, A_rshift, $1.exp, $3.exp);}
    ;
relational_expression:
      shift_expression                                      {$$ = $1;}
    | relational_expression CMP_LESS shift_expression       {$$.exp = _A_binop_exp(charPos, A_lt, $1.exp, $3.exp);}
    | relational_expression CMP_MORE shift_expression       {$$.exp = _A_binop_exp(charPos, A_gt, $1.exp, $3.exp);}
    | relational_expression CMP_LESS_EQ shift_expression    {$$.exp = _A_binop_exp(charPos, A_lte, $1.exp, $3.exp);}
    | relational_expression CMP_MORE_EQ shift_expression    {$$.exp = _A_binop_exp(charPos, A_gte, $1.exp, $3.exp);}
    ;
equality_expression:
      relational_expression                                 {$$ = $1;}
    | equality_expression CMP_EQ relational_expression      {$$.exp = _A_binop_exp(charPos, A_eq, $1.exp, $3.exp);}
    | equality_expression CMP_NEQ relational_expression     {$$.exp = _A_binop_exp(charPos, A_neq, $1.exp, $3.exp);}
    ;
AND_expression:
      equality_expression                                   {$$ = $1;}
    | AND_expression BIT_AND equality_expression            {$$.exp = _A_binop_exp(charPos, A_and, $1.exp, $3.exp);}
    ;
XOR_expression:
      AND_expression                                        {$$ = $1;}
    | XOR_expression BIT_XOR AND_expression                 {$$.exp = _A_binop_exp(charPos, A_xor, $1.exp, $3.exp);}
    ;
OR_expression:
      XOR_expression                                        {$$ = $1;}
    | OR_expression BIT_OR XOR_expression                   {$$.exp = _A_binop_exp(charPos, A_or, $1.exp, $3.exp);}
    ;
logical_AND_expression:
      OR_expression                                         {$$ = $1;}
    | logical_AND_expression LOG_AND OR_expression          {$$.exp = _A_binop_exp(charPos, A_log_and, $1.exp, $3.exp);}
    ;
logical_OR_expression:
      logical_AND_expression                                {$$ = $1;}
    | logical_OR_expression LOG_OR logical_AND_expression   {$$.exp = _A_binop_exp(charPos, A_log_or, $1.exp, $3.exp);}
    ;
conditional_expression:
      logical_OR_expression                                 {$$ = $1;}
    | logical_OR_expression QUEST_MARK expression COLON_MARK conditional_expression     {$$.exp = _A_threeop_exp(charPos, $1.exp, $3.exp, $5.exp);}
    ;
assignment_expression:
      conditional_expression                                    {$$ = $1;}
    | unary_expression assignment_operator assignment_expression    {$$.exp = _A_assign_exp(charPos, $1.exp, $2.op, $3.exp);}
    ;
assignment_operator:
      CAL_EQ            {$$.op = A_assign;}
    | CAL_TIMES_EQ      {$$.op = A_mula;}
    | CAL_DIV_EQ        {$$.op = A_diva;}
    | CAL_MOD_EQ        {$$.op = A_moda;}
    | CAL_PLUS_EQ       {$$.op = A_plusa;}
    | CAL_MINUS_EQ      {$$.op = A_minusa;}
    | CAL_LEFT_EQ       {$$.op = A_lshifta;}
    | CAL_RIGHT_EQ      {$$.op = A_rshifta;}
    | CAL_AND_EQ        {$$.op = A_anda;}
    | CAL_XOR_EQ        {$$.op = A_xora;}
    | CAL_OR_EQ         {$$.op = A_ora;}
    ;
expression:
      assignment_expression                         {$$ = $1;}
    | expression COMMA_MARK assignment_expression   {$$.exp = _A_seq_exp(charPos, $1.exp, $3.exp);}
    ;
constant_expression:
    conditional_expression  {$$ = $1;}
    ;




/*
declaration:
    declaration_specifiers init_declarator_list;    //todo
    ;
declaration_specifiers:
      storage_class_specifier declaration_specifiers  //todo
    | type_specifier declaration_specifiers   //todo
    | type_qualifier declaration_specifiers   //todo
    | function_specifier declaration_specifiers   //todo
    ;
init_declarator_list:
      init_declarator
    | init_declarator_list , init_declarator
    ;
init_declarator:
      declarator
    | declarator = initializer
    ;
storage_class_specifier:
    TYPEDEF
    EXTERN
    STATIC
    AUTO
    REGISTER
    ;
type_specifier:
    VOID
    CHAR
    SHORT
    INT
    LONG
    FLOAT
    DOUBLE
    SIGNED
    UNSIGNED
    struct_or_union_specifier
    enum_specifier
    typedef_name
    ;
struct_or_union_specifier:
    struct_or_union identifier { struct_declaration_list } //todo
    struct_or_union identifier
    ;
struct_or_union:
    STRUCT
    UNION
    ;
struct_declaration_list:
    struct_declaration
    struct_declaration_list struct_declaration
    ;
struct_declaration:
    specifier_qualifier_list struct_declarator_list ;
specifier_qualifier_list:
    type_specifier specifier_qualifier_list //todo
    type_qualifier specifier_qualifier_list //todo
    ;
struct_declarator_list:
    struct_declarator
    struct_declarator_list , struct_declarator
    ;
struct_declarator:
    declarator
    declarator : constant_expression //todo
    ;
enum_specifier:
    ENUM identifier {enumerator_list}//todo
    ENUM identifier {enumerator_list,}//todo
    ENUM identifier
    ;
enumerator_list:
    enumerator
    enumerator_list, enumerator
    ;
enumerator:
    enumeration_constant
    enumeration_constant = constant_expression
    ;
type_qualifier:
    CONST
    RESTRICT
    VOLATILE
    ;
function_specifier:
    INLINE
    ;
declarator:
    pointer direct_declarator//todo
    ;
direct_declarator:
    identifier
    (declarator)
    direct_declarator [ assignment_expression ] //todo
    direct_declarator [ * ]
    direct_declarator ( parameter_type_list )
    direct_declarator (identifier_list ) //todo
    ;
pointer:
    * type_qualifier_list //todo
    * type_qualifier_list pointer //todo
    ;
type_qualifier_list:
    type_qualifier
    type_qualifier_list type_qualifier
    ;
parameter_type_list:
    parameter_list
    parameter_list , ...
    ;
parameter_list:
    parameter_declaration
    parameter_list, parameter_declaration
    ;
parameter_declaration:
    declaration_specifiers declarator
    declaration_specifiers abstract_declarator //todo
    ;
identifier_list:
    identifier
    identifier_list , identifier
    ;
type_name:
    specifier_qualifier_list abstract_declarator //todo
    ;
abstract_declarator:
    pointer
    pointer direct_abstract_declarator //todo
    ;
direct_abstract_declarator:
    ( abstract_declarator)
    direct_abstract_declarator [ assignment_expression ]//todo
    direct_abstract_declarator [*]
    direct_abstract_declarator (parameter_type_list) //todo
    ;
typedef_name:
    identifier
    ;
initializer:
    assignment_expression
    { initializer_list}
    { initializer_list ,}
    ;
initializer_list:
    designation initializer //todo
    initializer_list, designation initializer //todo
    ;
designation:
    designator_list =
    ;
designator_list:
    designator
    designator_list designator
    ;
deisgnator:
    [constant_expression]
    .identifier
    ;







statement:
    labeled_statement
    compound_statement
    expression_statement
    selection_statement
    iteration_statement
    jump_statement
    ;
labeled_statement:
    identifier : statement
    case constant_expression : statement
    default : statement
    ;
compound_statement:
    { block_item_list } //todo
    ;
block_item_list:
    block_item
    block_item_list block_item
    ;
block_item:
    declaration
    statement
    ;
expression_statement:
    expression ; //todo
    ;
selection_statement:
    if ( expression ) statement
    if ( expression ) statement else statement
    switch ( expression ) statement
    ;
iteration_statement:
    while ( expression ) statement
    do statement while ( expression ) ;
    for ( expression ; expression ; expression ) statement //todo
    for ( declaration ; expression ; expression ) statement //todo
jump_statement:
    goto identifier ;
    continue ;
    break ;
    return expression ; //todo
    ;
*/
%%
void yyerror(char *s)
{
    printf("%s\n", s);
}