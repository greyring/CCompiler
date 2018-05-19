%{
#include "absync.h"
int yylex(void);
void yyerror(char *);
%}

%token  
%%
primary_expression:
      identifier    {$$ = $1;}
    | constant      {$$ = $1;}
    | string-literal{$$ = $1;}
    ;
postfix_expression:
      primary_expression    {$$ = A_simple_exp(pos, $1);}
    | (expression)          {$$ = $2;}
    | postfix_expression [expression]   
    | postfix_expression (expression)
    | postfix_expression () 
    | postfix_expression . idntifier
    | postfix_expression -> identifier
    | postfix_expression ++
    | postfix_expression --
    | (type_name) {initializer_list}
    | (type_name) {initializer_list,}
    ;
unary_expression:
      postfix_expression
    | ++ unary_expression
    | -- unary_expression
    | unary_operator cast_expression
    | sizeof unary_expression
    | sizeof (type_name)
    ;
unary_operator:
    & * + - ~ !
    ;
cast_expression:
      unary_expression
    | (type_name) cast_expression
    ;
multiplicative_expression:
      cast_expression
    | multiplicative_expression * cast_expression
    | multiplicative_expression / cast_expression
    | multiplicative_expression % cast_expression
    ;
additive_expression:
      multiplicative_expression
    | additive_expression + multiplicative_expression
    | additive_expression - multiplicative_expression
    ;
shift_expression:
      additive_expression
    | shift_expression << additive_expression
    | shift_expression >> additive_expression
    ;
relational_expression:
      shift_expression
    | relational_expression < shift_expression
    | relational_expression > shift_expression
    | relational_expression <= shift_expression
    | relational_expression >= shift_expression
    ;
equality_expression:
      relational_expression
    | equality_expression == relational_expression
    | equality_expression != relational_expression
    ;
AND_expression:
      equality_expression
    | AND_expression & equality_expression
    ;
XOR_expression:
      AND_expression
    | XOR_expression ^ AND_expression
    ;
OR_expression:
      XOR_expression
    | OR_expression | XOR_expression
    ;
logical_AND_expression:
      OR_expression
    | logical_AND_expression && OR_expression
    ;
logical_OR_expression:
      logical_AND_expression
    | logical_OR_expression || logical_AND_expression
    ;
conditional_expression:
      logical_OR_expression
    | logical_OR_expression ? expression : conditional_expression
    ;
assignment_expression:
      conditional_expression
    | unary_expression assignment_operator assign_expression
    ;
assignment_operator:
    = *= /= %= += -= <<= >>= &= ^= |=
    ;
expression:
      assignment_expression
    | expression, assignment_expression
    ;
constant_expression:
    conditional_expression
    ;





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
    direct_abstract_declarator [ assign_expression ]//todo
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
%%