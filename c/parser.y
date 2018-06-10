%locations
%define parse.error verbose

%{
#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
#include "absync.h"
extern int charPos; 
//#define charPos yylloc
void yyerror(const char *);
int yylex(void);
A_def root;
%}

%union {int ival; char cval; char* sval; double fval; A_op op; A_storage_type storage_type; A_prim_type prim_type; A_qual_type qual_type; A_func_type func_type; A_exp exp; A_spec spec; A_type type; A_pointer pointer_; A_param param; A_dec dec; A_declaration declaration; A_type_name type_name_; A_designator designator_; A_init init; A_stat stat; A_def def;}

%token KEY_AUTO KEY_BREAK KEY_CASE KEY_CHAR KEY_CONST KEY_CONTINUE KEY_DEFAULT KEY_DO KEY_DOUBLE KEY_ELSE KEY_ENUM KEY_EXTERN KEY_FLOAT KEY_FOR KEY_GOTO KEY_IF KEY_INLINE KEY_INT KEY_LONG KEY_REGISTER KEY_RESTRICT KEY_RETURN KEY_SHORT KEY_SIGNED KEY_SIZEOF KEY_STATIC KEY_STRUCT KEY_SWITCH KEY_TYPEDEF KEY_UNION KEY_UNSIGNED KEY_VOID KEY_VOLATILE KEY_WHILE KEY_UL_BOOL KEY_UL_COMPLEX KEY_UL_IMAGINARY LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE LSHIFT RSHIFT DOT DOLLAR ARROW DOUBLE_PLUS DOUBLE_MINUS BIT_AND BIT_NOT BIT_XOR BIT_OR OPT_TIMES OPT_PLUS OPT_MINUS OPT_DIV OPT_MOD CMP_LESS CMP_MORE CMP_LESS_EQ CMP_MORE_EQ CMP_EQ CMP_NEQ LOG_NOT LOG_AND LOG_OR QUEST_MARK COLON_MARK SEMIC_MARK ELLIP_MARK COMMA_MARK CAL_EQ CAL_TIMES_EQ CAL_DIV_EQ CAL_MOD_EQ CAL_PLUS_EQ CAL_MINUS_EQ CAL_LEFT_EQ CAL_RIGHT_EQ CAL_AND_EQ CAL_XOR_EQ CAL_OR_EQ SHARP_MARK DOUBLE_SHARP

%token <sval> IDENTIFIER STR_LITERAL
%token <ival> INT_DEC_CONST INT_OCT_CONST INT_HEX_CONST 
%token <fval> FLT_DEC_CONST FLT_HEX_CONST 
%token <cval> CHAR_CONST 
%type  <exp> primary_expression postfix_expression unary_expression cast_expression multiplicative_expression additive_expression shift_expression relational_expression equality_expression AND_expression XOR_expression OR_expression logical_AND_expression logical_OR_expression conditional_expression assignment_expression expression constant_expression identifier_list
%type <op> unary_operator assignment_operator
%type <dec> init_declarator_list init_declarator struct_declarator_list struct_declarator declarator direct_declarator abstract_declarator direct_abstract_declarator
%type <declaration> declaration struct_declaration_list struct_declaration declaration_list
%type <spec> declaration_specifiers storage_class_specifier type_specifier specifier_qualifier_list type_qualifier function_specifier type_qualifier_list
%type <type> struct_or_union_specifier 
%type <type> enum_specifier typedef_name
%type <prim_type> struct_or_union
%type <init> enumerator_list enumerator initializer initializer_list
%type <pointer_> pointer
%type <param> parameter_type_list parameter_list parameter_declaration
%type <type_name_> type_name
%type <designator_> designation designator_list designator
%type <stat> statement labeled_statement compound_statement block_item_list block_item expression_statement selection_statement iteration_statement jump_statement
%type <def> start translation_unit external_declaration function_definition

%nonassoc LOWER_THEN_ELSE
%nonassoc KEY_ELSE

%%
start:
    translation_unit {root = $$ = $1;}
    ;
primary_expression:
      IDENTIFIER            {$$ = _A_id_exp(charPos, _S_symbol($1));}
    | INT_DEC_CONST         {$$ = _A_intexp_exp(charPos, $1);}
    | INT_OCT_CONST         {$$ = _A_intexp_exp(charPos, $1);}
    | INT_HEX_CONST         {$$ = _A_intexp_exp(charPos, $1);}
    | FLT_DEC_CONST         {$$ = _A_floatexp_exp(charPos, $1);}
    | FLT_HEX_CONST         {$$ = _A_floatexp_exp(charPos, $1);}
    | CHAR_CONST            {$$ = _A_charexp_exp(charPos, $1);}
    | STR_LITERAL           {$$ = _A_strexp_exp(charPos, $1);}
    ;

postfix_expression:
      primary_expression                                                {$$ = $1;}
    | LPAREN expression RPAREN                                          {$$ = $2;}
    | postfix_expression LBRACKET expression RBRACKET                   {$$ = _A_subscript_exp(charPos, $1, $3);}   
    | postfix_expression LPAREN expression RPAREN                       {$$ = _A_funccall_exp(charPos, $1, $3);}
    | postfix_expression LPAREN RPAREN                                  {$$ = _A_funccall_exp(charPos, $1, NULL);}
    | postfix_expression DOT IDENTIFIER                                 {$$ = _A_dot_exp(charPos, $1, _S_symbol($3));}
    | postfix_expression ARROW IDENTIFIER                               {$$ = _A_point_exp(charPos, $1, _S_symbol($3));}
    | postfix_expression DOUBLE_PLUS                                    {$$ = _A_postpp_exp(charPos, $1);}
    | postfix_expression DOUBLE_MINUS                                   {$$ = _A_postmm_exp(charPos, $1);}
    | LPAREN type_name RPAREN LBRACE initializer_list RBRACE            {$$ = _A_init_exp(charPos, $2, $5);}
    | LPAREN type_name RPAREN LBRACE initializer_list COMMA_MARK RBRACE    {$$ = _A_init_exp(charPos, $2, $5);}
    ;

unary_expression:
      postfix_expression                {$$ = $1;}
    | DOUBLE_PLUS unary_expression      {$$ = _A_prepp_exp(charPos, $2);}
    | DOUBLE_MINUS unary_expression     {$$ = _A_premm_exp(charPos, $2);}
    | unary_operator cast_expression    {$$ = _A_unaryop_exp(charPos, $1, $2);}
    | KEY_SIZEOF unary_expression       {$$ = _A_sizeof_unary_exp(charPos, $2);}
    | KEY_SIZEOF LPAREN type_name RPAREN {$$ = _A_sizeof_type_exp(charPos, $3);}
    ;
unary_operator:
      BIT_AND       {$$ = A_AND;}
    | OPT_TIMES     {$$ = A_MUL;}
    | OPT_PLUS      {$$ = A_PLUS;}
    | OPT_MINUS     {$$ = A_MINUS;}
    | BIT_NOT       {$$ = A_NOT;}
    | LOG_NOT       {$$ = A_LOGNOT;}
    ;
cast_expression:
      unary_expression                                  {$$ = $1;}
    | LPAREN type_name RPAREN cast_expression         {$$ = _A_cast_exp(charPos, $2, $4);}
    ;
multiplicative_expression:
      cast_expression                                       {$$ = $1;}
    | multiplicative_expression OPT_TIMES cast_expression   {$$ = _A_binop_exp(charPos, A_MUL, $1, $3);}
    | multiplicative_expression OPT_DIV cast_expression     {$$ = _A_binop_exp(charPos, A_DIV, $1, $3);}
    | multiplicative_expression OPT_MOD cast_expression     {$$ = _A_binop_exp(charPos, A_MOD, $1, $3);}
    ;
additive_expression:
      multiplicative_expression
    | additive_expression OPT_PLUS multiplicative_expression    {$$ = _A_binop_exp(charPos, A_PLUS, $1, $3);}
    | additive_expression OPT_MINUS multiplicative_expression   {$$ = _A_binop_exp(charPos, A_MINUS, $1, $3);}
    ;
shift_expression:
      additive_expression                           {$$ = $1;}
    | shift_expression LSHIFT additive_expression   {$$ = _A_binop_exp(charPos, A_LSHIFT, $1, $3);}
    | shift_expression RSHIFT additive_expression   {$$ = _A_binop_exp(charPos, A_RSHIFT, $1, $3);}
    ;
relational_expression:
      shift_expression                                      {$$ = $1;}
    | relational_expression CMP_LESS shift_expression       {$$ = _A_binop_exp(charPos, A_LT, $1, $3);}
    | relational_expression CMP_MORE shift_expression       {$$ = _A_binop_exp(charPos, A_GT, $1, $3);}
    | relational_expression CMP_LESS_EQ shift_expression    {$$ = _A_binop_exp(charPos, A_LTE, $1, $3);}
    | relational_expression CMP_MORE_EQ shift_expression    {$$ = _A_binop_exp(charPos, A_GTE, $1, $3);}
    ;
equality_expression:
      relational_expression                                 {$$ = $1;}
    | equality_expression CMP_EQ relational_expression      {$$ = _A_binop_exp(charPos, A_EQ, $1, $3);}
    | equality_expression CMP_NEQ relational_expression     {$$ = _A_binop_exp(charPos, A_NEQ, $1, $3);}
    ;
AND_expression:
      equality_expression                                   {$$ = $1;}
    | AND_expression BIT_AND equality_expression            {$$ = _A_binop_exp(charPos, A_AND, $1, $3);}
    ;
XOR_expression:
      AND_expression                                        {$$ = $1;}
    | XOR_expression BIT_XOR AND_expression                 {$$ = _A_binop_exp(charPos, A_XOR, $1, $3);}
    ;
OR_expression:
      XOR_expression                                        {$$ = $1;}
    | OR_expression BIT_OR XOR_expression                   {$$ = _A_binop_exp(charPos, A_OR, $1, $3);}
    ;
logical_AND_expression:
      OR_expression                                         {$$ = $1;}
    | logical_AND_expression LOG_AND OR_expression          {$$ = _A_binop_exp(charPos, A_LOGAND, $1, $3);}
    ;
logical_OR_expression:
      logical_AND_expression                                {$$ = $1;}
    | logical_OR_expression LOG_OR logical_AND_expression   {$$ = _A_binop_exp(charPos, A_LOGOR, $1, $3);}
    ;
conditional_expression:
      logical_OR_expression                                 {$$ = $1;}
    | logical_OR_expression QUEST_MARK expression COLON_MARK conditional_expression     {$$ = _A_threeop_exp(charPos, $1, $3, $5);}
    ;
assignment_expression:
      conditional_expression                                        {$$ = $1;}
    | unary_expression assignment_operator assignment_expression    {$$ = _A_assign_exp(charPos, $1, $2, $3);}
    ;
assignment_operator:
      CAL_EQ            {$$ = A_ASSIGN;}
    | CAL_TIMES_EQ      {$$ = A_MULA;}
    | CAL_DIV_EQ        {$$ = A_DIVA;}
    | CAL_MOD_EQ        {$$ = A_MODA;}
    | CAL_PLUS_EQ       {$$ = A_PLUSA;}
    | CAL_MINUS_EQ      {$$ = A_MINUSA;}
    | CAL_LEFT_EQ       {$$ = A_LSHIFTA;}
    | CAL_RIGHT_EQ      {$$ = A_RSHIFTA;}
    | CAL_AND_EQ        {$$ = A_ANDA;}
    | CAL_XOR_EQ        {$$ = A_XORA;}
    | CAL_OR_EQ         {$$ = A_ORA;}
    ;
expression:
      assignment_expression                         {$$ = $1;}
    | expression COMMA_MARK assignment_expression   {$$ = _A_seq_exp(charPos, $1, $3);}
    ;
constant_expression:
    conditional_expression  {$$ = $1;}
    ;




declaration:                           
    declaration_specifiers init_declarator_list SEMIC_MARK  {$$ = _A_simple_declaration(charPos, $1, $2);}
    | declaration_specifiers SEMIC_MARK                     {$$ = _A_simple_declaration(charPos, $1, NULL);}
    ;
declaration_specifiers:
      storage_class_specifier declaration_specifiers    {$$ = _A_seq_spec(charPos, $1, $2);}
    | type_specifier declaration_specifiers             {$$ = _A_seq_spec(charPos, $1, $2);}
    | type_qualifier declaration_specifiers             {$$ = _A_seq_spec(charPos, $1, $2);}
    | function_specifier declaration_specifiers         {$$ = _A_seq_spec(charPos, $1, $2);}
    | storage_class_specifier                           {$$ = $1;}
    | type_specifier                                    {$$ = $1;}
    | type_qualifier                                    {$$ = $1;}
    | function_specifier                                {$$ = $1;}
    ;
init_declarator_list:
      init_declarator                                   {$$ = $1;}
    | init_declarator_list COMMA_MARK init_declarator   {$$ = _A_seq_dec(charPos, $1, $3);}
    ;
init_declarator:
      declarator                    {$$ = $1;}
    | declarator CAL_EQ initializer {$$ = _A_init_dec(charPos, $1, $3);}
    ;
storage_class_specifier:
      KEY_TYPEDEF       {$$ = _A_storage_spec(charPos, A_TYPEDEF);}
    | KEY_EXTERN        {$$ = _A_storage_spec(charPos, A_EXTERN);}
    | KEY_STATIC        {$$ = _A_storage_spec(charPos, A_STATIC);}
    | KEY_AUTO          {$$ = _A_storage_spec(charPos, A_AUTO);}
    | KEY_REGISTER      {$$ = _A_storage_spec(charPos, A_REGISTER);}
    ;
type_specifier:
      KEY_VOID                      {$$ = _A_type_spec(charPos, _A_simple_type(charPos, A_VOID));}
    | KEY_CHAR                      {$$ = _A_type_spec(charPos, _A_simple_type(charPos, A_CHAR));}
    | KEY_SHORT                     {$$ = _A_type_spec(charPos, _A_simple_type(charPos, A_SHORT));}
    | KEY_INT                       {$$ = _A_type_spec(charPos, _A_simple_type(charPos, A_INT));}
    | KEY_LONG                      {$$ = _A_type_spec(charPos, _A_simple_type(charPos, A_LONG));}
    | KEY_FLOAT                     {$$ = _A_type_spec(charPos, _A_simple_type(charPos, A_FLOAT));}
    | KEY_DOUBLE                    {$$ = _A_type_spec(charPos, _A_simple_type(charPos, A_DOUBLE));}
    | KEY_SIGNED                    {$$ = _A_type_spec(charPos, _A_simple_type(charPos, A_SIGNED));}
    | KEY_UNSIGNED                  {$$ = _A_type_spec(charPos, _A_simple_type(charPos, A_UNSIGNED));}
    | struct_or_union_specifier     {$$ = _A_type_spec(charPos, $1);}
    | enum_specifier                {$$ = _A_type_spec(charPos, $1);}
    | DOLLAR typedef_name           {$$ = _A_type_spec(charPos, $2);}
    ;
struct_or_union_specifier:
      struct_or_union IDENTIFIER LBRACE struct_declaration_list RBRACE  {$$ = _A_struct_union_type(charPos, $1, _S_symbol($2), $4);}
    | struct_or_union LBRACE struct_declaration_list RBRACE             {$$ = _A_struct_union_type(charPos, $1, NULL, $3);}
    | struct_or_union IDENTIFIER                                        {$$ = _A_struct_union_type(charPos, $1, _S_symbol($2), NULL);}
    ;
struct_or_union:
    KEY_STRUCT      {$$ = A_STRUCT;}
    | KEY_UNION     {$$ = A_UNION;}
    ;
struct_declaration_list:
      struct_declaration                            {$$ = $1;}
    | struct_declaration_list struct_declaration    {$$ = _A_seq_declaration(charPos, $1, $2);}
    ;
struct_declaration:
    specifier_qualifier_list struct_declarator_list SEMIC_MARK  {$$ = _A_simple_declaration(charPos, $1, $2);}
    ;
specifier_qualifier_list:
      type_specifier specifier_qualifier_list   {$$ = _A_seq_spec(charPos, $1, $2);}
    | type_qualifier specifier_qualifier_list   {$$ = _A_seq_spec(charPos, $1, $2);}
    | type_specifier                            {$$ = $1;}
    | type_qualifier                            {$$ = $1;}
    ;
struct_declarator_list:
      struct_declarator                                     {$$ = $1;}
    | struct_declarator_list COMMA_MARK struct_declarator   {$$ = _A_seq_dec(charPos, $1, $3);}
    ;
struct_declarator:
      declarator                                    {$$ = $1;}
    | declarator COLON_MARK constant_expression     {$$ = _A_bit_dec(charPos, $1, $3);}
    | COLON_MARK constant_expression                {$$ = _A_bit_dec(charPos, NULL, $2);}
    ;
enum_specifier:
      KEY_ENUM IDENTIFIER LBRACE enumerator_list RBRACE               {$$ = _A_enumtype_type(charPos, _S_symbol($2), $4);}
    | KEY_ENUM IDENTIFIER LBRACE enumerator_list COMMA_MARK RBRACE    {$$ = _A_enumtype_type(charPos, _S_symbol($2), $4);}
    | KEY_ENUM LBRACE enumerator_list RBRACE                          {$$ = _A_enumtype_type(charPos, NULL, $3);}
    | KEY_ENUM LBRACE enumerator_list COMMA_MARK RBRACE               {$$ = _A_enumtype_type(charPos, NULL, $3);}
    | KEY_ENUM IDENTIFIER                                             {$$ = _A_enumtype_type(charPos, _S_symbol($2), NULL);}
    ;
enumerator_list:
      enumerator                                {$$ = $1;}
    | enumerator_list COMMA_MARK enumerator     {$$ = _A_seq_init(charPos, $1, $3);}
    ;
enumerator:
      IDENTIFIER                                {$$ = _A_enumtype_init(charPos, _S_symbol($1), NULL);}
    | IDENTIFIER CMP_EQ constant_expression     {$$ = _A_enumtype_init(charPos, _S_symbol($1), $3);}
    ;
type_qualifier:
      KEY_CONST         {$$ = _A_qual_type_spec(charPos, A_CONST);}
    | KEY_RESTRICT      {$$ = _A_qual_type_spec(charPos, A_RESTRICT);}
    | KEY_VOLATILE      {$$ = _A_qual_type_spec(charPos, A_VOLATILE);}
    ;
function_specifier:
    KEY_INLINE          {$$ = _A_func_type_spec(charPos, A_INLINE);}
    ;
declarator:
      pointer direct_declarator     {$$ = _A_pointer_dec(charPos, $1, $2);}
    | direct_declarator             {$$ = $1;}
    ;
direct_declarator:
      IDENTIFIER                                                {$$ = _A_simple_dec(charPos, _S_symbol($1));}      
    | LPAREN declarator RPAREN                                  {$$ = $2;}
    | direct_declarator LBRACKET assignment_expression RBRACKET {$$ = _A_array_dec(charPos, $1, $3);}
    | direct_declarator LBRACKET OPT_TIMES RBRACKET             {$$ = _A_array_proto_dec(charPos, $1);}
    | direct_declarator LBRACKET RBRACKET                       {$$ = _A_array_dec(charPos, $1, NULL);}
    | direct_declarator LPAREN parameter_type_list RPAREN       {$$ = _A_func_dec(charPos, $1, $3);}
    | direct_declarator LPAREN identifier_list RPAREN           {$$ = _A_funcid_dec(charPos, $1, $3);}
    | direct_declarator LPAREN RPAREN                           {$$ = _A_func_dec(charPos, $1, NULL);}
    ;
pointer://for const pointer
      OPT_TIMES type_qualifier_list         {$$ = _A_simple_pointer(charPos, $2);}
    | OPT_TIMES type_qualifier_list pointer {$$ = _A_seq_pointer(charPos, _A_simple_pointer(charPos, $2), $3);}
    | OPT_TIMES pointer                     {$$ = _A_seq_pointer(charPos, NULL, $2);}
    | OPT_TIMES                             {$$ = _A_simple_pointer(charPos, NULL);}
    ;
type_qualifier_list:
      type_qualifier                        {$$ = $1;}
    | type_qualifier_list type_qualifier    {$$ = _A_seq_spec(charPos, $1, $2);}
    ;
parameter_type_list:
      parameter_list                        {$$ = $1;}
    | parameter_list COMMA_MARK ELLIP_MARK  {$$ = _A_uncertain_param(charPos, $1);}
    ;
parameter_list:
      parameter_declaration                             {$$ = $1;}
    | parameter_list COMMA_MARK parameter_declaration   {$$ = _A_seq_param(charPos, $1, $3);}
    ;
parameter_declaration:
      declaration_specifiers declarator                 {$$ = _A_dec_param(charPos, $1, $2);}
    | declaration_specifiers abstract_declarator        {$$ = _A_dec_param(charPos, $1, $2);}
    | declaration_specifiers                            {$$ = _A_dec_param(charPos, $1, NULL);}
    ;
identifier_list:
      IDENTIFIER                                        {$$ = _A_id_exp(charPos, _S_symbol($1));}
    | identifier_list COMMA_MARK IDENTIFIER             {$$ = _A_seq_exp(charPos, $1, _A_id_exp(charPos, _S_symbol($3)));}
    ;
type_name:
      specifier_qualifier_list abstract_declarator      {$$ = _A_simple_type_name(charPos, $1, $2);}
    | specifier_qualifier_list                          {$$ = _A_simple_type_name(charPos, $1, NULL);}
    ;
abstract_declarator:
      pointer                               {$$ = _A_pointer_dec(charPos, $1, NULL);}
    | pointer direct_abstract_declarator    {$$ = _A_pointer_dec(charPos, $1, $2);}
    | direct_abstract_declarator            {$$ = $1;}
    ;
direct_abstract_declarator://no id
      LPAREN abstract_declarator RPAREN                                     {$$ = $2;}
    | direct_abstract_declarator LBRACKET assignment_expression RBRACKET    {$$ = _A_array_dec(charPos, $1, $3);}
    | direct_abstract_declarator LBRACKET RBRACKET                          {$$ = _A_array_dec(charPos, $1, NULL);}
    | LBRACKET assignment_expression RBRACKET                               {$$ = _A_array_dec(charPos, NULL, $2);}
    | LBRACKET RBRACKET                                                     {$$ = _A_array_dec(charPos, NULL, NULL);}
    | direct_abstract_declarator LBRACKET OPT_TIMES RBRACKET                {$$ = _A_array_proto_dec(charPos, $1);}
    | direct_abstract_declarator LPAREN parameter_type_list RPAREN          {$$ = _A_func_dec(charPos, $1, $3);}
    | direct_abstract_declarator LPAREN RPAREN                              {$$ = _A_func_dec(charPos, $1, NULL);}
    | LPAREN parameter_type_list RPAREN                                     {$$ = _A_func_dec(charPos, NULL, $2);}
    | LPAREN RPAREN                                                         {$$ = _A_func_dec(charPos, NULL, NULL);}
    ;
typedef_name:
    IDENTIFIER  {$$ = _A_typeid_type(charPos, _S_symbol($1));}
    ;
initializer:
      assignment_expression                         {$$ = _A_simple_init(charPos, $1);}
    | LBRACE initializer_list RBRACE                {$$ = $2;}
    | LBRACE initializer_list COMMA_MARK RBRACE     {$$ = $2;}
    ;
initializer_list:
      designation initializer                               {$$ = _A_designation_init(charPos, $1, $2);}
    | initializer                                           {$$ = $1;}
    | initializer_list COMMA_MARK designation initializer   {$$ = _A_seq_init(charPos, $1, _A_designation_init(charPos, $3, $4));}
    | initializer_list COMMA_MARK initializer               {$$ = _A_seq_init(charPos, $1, $3);}
    ;
designation:
      designator_list CMP_EQ                    {$$ = $1;}
    ;
designator_list:
      designator                                {$$ = $1;}
    | designator_list designator                {$$ = _A_seq_designator(charPos, $1, $2);}
    ;
designator:
      LBRACKET constant_expression RBRACKET     {$$ = _A_subscript_designator(charPos, $2);}
    | DOT IDENTIFIER                            {$$ = _A_dot_designator(charPos, _S_symbol($2));}
    ;






statement:
      labeled_statement     {$$ = $1;}
    | compound_statement    {$$ = $1;}
    | expression_statement  {$$ = $1;}
    | selection_statement   {$$ = $1;}
    | iteration_statement   {$$ = $1;}
    | jump_statement        {$$ = $1;}
    ;
labeled_statement:
      IDENTIFIER COLON_MARK statement                     {$$ = _A_label_stat(charPos, _S_symbol($1), $3);}
    | KEY_CASE constant_expression COLON_MARK statement   {$$ = _A_casestat_stat(charPos, $2, $4);}
    | KEY_DEFAULT COLON_MARK statement                    {$$ = _A_defaultstat_stat(charPos, $3);}
    ;
compound_statement:
      LBRACE block_item_list RBRACE     {$$ = _A_block_stat(charPos, $2);}
    | LBRACE RBRACE                     {;}
    ;
block_item_list:
      block_item                        {$$ = $1;}
    | block_item_list block_item        {$$ = _A_seq_stat(charPos, $1, $2);}
    ;
block_item:
      declaration                       {$$ = _A_dec_stat(charPos, $1);}
    | statement                         {$$ = $1;}
    ;
expression_statement:
      expression SEMIC_MARK             {$$ = _A_exp_stat(charPos, $1);}
    | SEMIC_MARK                        {;}
    ;
selection_statement:
      KEY_IF LPAREN expression RPAREN statement %prec LOWER_THEN_ELSE   {$$ = _A_ifstat_stat(charPos, $3, $5, NULL);} 
    | KEY_IF LPAREN expression RPAREN statement KEY_ELSE statement      {$$ = _A_ifstat_stat(charPos, $3, $5, $7);}
    | KEY_SWITCH LPAREN expression RPAREN statement                     {$$ = _A_switchstat_stat(charPos, $3, $5);}
    ;
iteration_statement:
      KEY_WHILE LPAREN expression RPAREN statement                                              {$$ = _A_whilestat_stat(charPos, $3, $5);}
    | KEY_DO statement KEY_WHILE LPAREN expression RPAREN SEMIC_MARK                            {$$ = _A_dowhile_stat(charPos, $2, $5);}
    | KEY_FOR LPAREN expression SEMIC_MARK expression SEMIC_MARK expression RPAREN statement    {$$ = _A_forexp_stat(charPos, $3, $5, $7, $9);}
    | KEY_FOR LPAREN expression SEMIC_MARK expression SEMIC_MARK RPAREN statement               {$$ = _A_forexp_stat(charPos, $3, $5, NULL, $8);}
    | KEY_FOR LPAREN expression SEMIC_MARK SEMIC_MARK expression RPAREN statement               {$$ = _A_forexp_stat(charPos, $3, $6, NULL, $8);}
    | KEY_FOR LPAREN SEMIC_MARK expression SEMIC_MARK expression RPAREN statement               {$$ = _A_forexp_stat(charPos, NULL, $4, $6, $8);}
    | KEY_FOR LPAREN expression SEMIC_MARK SEMIC_MARK RPAREN statement                          {$$ = _A_forexp_stat(charPos, $3, NULL, NULL, $7);}
    | KEY_FOR LPAREN SEMIC_MARK expression SEMIC_MARK RPAREN statement                          {$$ = _A_forexp_stat(charPos, NULL, $4, NULL, $7);}
    | KEY_FOR LPAREN SEMIC_MARK SEMIC_MARK expression RPAREN statement                          {$$ = _A_forexp_stat(charPos, NULL, NULL, $5, $7);}
    | KEY_FOR LPAREN SEMIC_MARK SEMIC_MARK RPAREN statement                                     {$$ = _A_forexp_stat(charPos, NULL, NULL, NULL, $6);}
    | KEY_FOR LPAREN declaration SEMIC_MARK expression SEMIC_MARK expression RPAREN statement   {$$ = _A_fordec_stat(charPos, $3, $5, $7, $9);}
    | KEY_FOR LPAREN declaration SEMIC_MARK expression SEMIC_MARK RPAREN statement              {$$ = _A_fordec_stat(charPos, $3, $5, NULL, $8);}
    | KEY_FOR LPAREN declaration SEMIC_MARK SEMIC_MARK expression RPAREN statement              {$$ = _A_fordec_stat(charPos, $3, NULL, $6, $8);}
    | KEY_FOR LPAREN declaration SEMIC_MARK SEMIC_MARK RPAREN statement                         {$$ = _A_fordec_stat(charPos, $3, NULL, NULL, $7);}
    ;
jump_statement:
      KEY_GOTO IDENTIFIER SEMIC_MARK    {$$ = _A_gotostat_stat(charPos, _S_symbol($2));}
    | KEY_CONTINUE SEMIC_MARK           {$$ = _A_continuestat_stat(charPos);}
    | KEY_BREAK SEMIC_MARK              {$$ = _A_breakstat_stat(charPos);}
    | KEY_RETURN expression SEMIC_MARK  {$$ = _A_returnstat_stat(charPos, $2);}
    | KEY_RETURN SEMIC_MARK             {$$ = _A_returnstat_stat(charPos, NULL);}
    ;



translation_unit:
      external_declaration                                                      {$$ = $1;}
    | translation_unit external_declaration                                     {$$ = _A_seq_def(charPos, $1, $2);}
    ;
external_declaration:
      function_definition                                                       {$$ = $1;}
    | declaration                                                               {$$ = _A_simple_def(charPos, $1);}
    ;
function_definition:
      declaration_specifiers declarator declaration_list compound_statement     {$$ = _A_func_def(charPos, $1, $2, $3, $4);}
    | declaration_specifiers declarator compound_statement                      {$$ = _A_func_def(charPos, $1, $2, NULL, $3);}
    ;
declaration_list:
      declaration                                                               {$$ = $1;}
    | declaration_list declaration                                              {$$ = _A_seq_declaration(charPos, $1, $2);}
    ;
%%
void yyerror(const char *s)
{
    printf("%d line %d:%d to line %d:%d :%s\n", charPos, yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column, s);
}
