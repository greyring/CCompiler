%{
    #include "absync.h"
    #include "y.tab.h"
    #include "util.h"
    #include <stdio.h>
    #include <string.h>
    int charPos = 0;
    #define ADJ (charPos+=yyleng)
    //#define ADJ 

    static void cstrcpy(char* s){
        int len;
        
        len = strlen(s);
        yylval.sval = checked_malloc(sizeof(char) * len-1);
        yylval.sval[len-2] = '\0';
        for (int i=0; i<len-2; i++){
            yylval.sval[i] = s[i+1];
        }
    }
    static void cstrcpy2(char* s){
        yylval.sval = checked_malloc(sizeof(char) * strlen(s) + 1);
        strcpy(yylval.sval, s);
    }
    #define YY_USER_ACTION if (yylloc.first_line != yylineno) \
                                yylloc.first_column = yylloc.last_column = 0;\
                           else\
                                yylloc.first_column = yylloc.last_column += yyleng;\
                           yylloc.first_line = yylloc.last_line = yylineno;
%}

%option yylineno

/*
    keyword
*/

key_auto          "auto"
key_break         "break"
key_case          "case"
key_char          "char"
key_const         "const"
key_continue      "continue"
key_default       "default"
key_do            "do"
key_double        "double"
key_else          "else"
key_enum          "enum"
key_extern        "extern"
key_float         "float"
key_for           "for"
key_goto          "goto"
key_if            "if"
key_inline        "inline"
key_int           "int"
key_long          "long"
key_register      "register"
key_restrict      "restrict"
key_return        "return"
key_short         "short"
key_signed        "signed"
key_sizeof        "sizeof"
key_static        "static"
key_struct        "struct"
key_switch        "switch"
key_typedef       "typedef"
key_union         "union"
key_unsigned      "unsigned"
key_void          "void"
key_volatile      "volatile"
key_while         "while"
key_ul_bool       "_Bool"
key_ul_complex    "_Complex"
key_ul_Imaginary  "_Imaginary"


/*
    identifier
*/
identifier       [a-zA-Z_][a-zA-Z0-9_]*

/*
    constant
*/
int_dec_const    0|[1-9][0-9]*((u|U)?|(l|L)?|(ll|LL)?)
int_oct_const    0(0|([1-7][0-7]*))((u|U)?|(l|L)?|(ll|LL)?)
int_hex_const    ("0x"|"0X")(0|([1-9a-fA-F][0-9a-fA-F]*))((u|U)?|(l|L)?|(ll|LL)?)

flt_dec_const    (((([0-9]*"."[0-9]+))|([0-9]+"."))([eE][+-]?[0-9]+)?[flFL]?|([0-9]+[eE][+-]?[0-9]+[flFL]?))
flt_hex_const    ("0x"|"0X")((([0-9a-fA-F]*"."[0-9a-fA-F]+)|([0-9a-fA-F]+"."))|([0-9a-fA-F]+))([pP][+-]?[0-9]+)[flFL]?

char_const       L?\'([^\'\\\n]|((\\[\'\"\?\\abfnrtv])|(\[0-7]{1,3})|(\\x[0-9a-fA-F]+)))+\'

/*
    string-literal
*/
str_literal     L?\"([^\"\\\n]|(\\[\'\"\?\\abfnrtv]))*\"


/*
    punctuator
*/
lparen             "("
rparen             ")"
lbracket           "["
rbracket           "]"
lbrace             "{"
rbrace             "}"
lshift             "<<"
rshift             ">>"

dot                "\."
dollar             "$"

arrow              "->"

double_plus        "++"
double_minus       "--"

bit_and            "&"
bit_not            "~"
bit_xor            "^"
bit_or             "|"

opt_times          "*"
opt_plus           "+"
opt_minus          "-"
opt_div            "/"
opt_mod            "%"

cmp_less           "<"
cmp_more           ">"
cmp_less_eq        "<="
cmp_more_eq        ">="
cmp_eq             "=="
cmp_neq            "!="

log_not            "!"
log_and            "&&"
log_or             "||"

quest_mark         "?"
colon_mark         ":"
semic_mark         ";"
ellip_mark         "\.\.\."
comma_mark         ","

cal_eq             "="
cal_times_eq       "*="
cal_div_eq         "/="
cal_mod_eq         "%="
cal_plus_eq        "+="
cal_minus_eq       "-="
cal_left_eq        "<<="
cal_right_eq       ">>="
cal_and_eq         "&="
cal_xor_eq         "^="
cal_or_eq          "|="

sharp_mark         "#"
double_sharp       "##"









/*
    others
*/
blank_space      " "
enter            "\n"
cline            "\r"
tab              "\t"
default          .

%%

{key_auto}          {ADJ; return KEY_AUTO;}
{key_break}         {ADJ; return KEY_BREAK;}
{key_case}          {ADJ; return KEY_CASE;}
{key_char}          {ADJ; return KEY_CHAR;}
{key_const}         {ADJ; return KEY_CONST;}
{key_continue}      {ADJ; return KEY_CONTINUE;}
{key_default}       {ADJ; return KEY_DEFAULT;}
{key_do}            {ADJ; return KEY_DO;}
{key_double}        {ADJ; return KEY_DOUBLE;}
{key_else}          {ADJ; return KEY_ELSE;}
{key_enum}          {ADJ; return KEY_ENUM;}
{key_extern}        {ADJ; return KEY_EXTERN;}
{key_float}         {ADJ; return KEY_FLOAT;}
{key_for}           {ADJ; return KEY_FOR;}
{key_goto}          {ADJ; return KEY_GOTO;}
{key_if}            {ADJ; return KEY_IF;}
{key_inline}        {ADJ; return KEY_INLINE;}
{key_int}           {ADJ; return KEY_INT;}
{key_long}          {ADJ; return KEY_LONG;}
{key_register}      {ADJ; return KEY_REGISTER;}
{key_restrict}      {ADJ; return KEY_RESTRICT;}
{key_return}        {ADJ; return KEY_RETURN;}
{key_short}         {ADJ; return KEY_SHORT;}
{key_signed}        {ADJ; return KEY_SIGNED;}
{key_sizeof}        {ADJ; return KEY_SIZEOF;}
{key_static}        {ADJ; return KEY_STATIC;}
{key_struct}        {ADJ; return KEY_STRUCT;}
{key_switch}        {ADJ; return KEY_SWITCH;}
{key_typedef}       {ADJ; return KEY_TYPEDEF;}
{key_union}         {ADJ; return KEY_UNION;}
{key_unsigned}      {ADJ; return KEY_UNSIGNED;}
{key_void}          {ADJ; return KEY_VOID;}
{key_volatile}      {ADJ; return KEY_VOLATILE;}
{key_while}         {ADJ; return KEY_WHILE;}
{key_ul_bool}       {ADJ; return KEY_UL_BOOL;}
{key_ul_complex}    {ADJ; return KEY_UL_COMPLEX;}
{key_ul_Imaginary}  {ADJ; return KEY_UL_IMAGINARY;}


{identifier}        {ADJ; cstrcpy2(yytext); return IDENTIFIER;}


{int_dec_const}     {ADJ; sscanf(yytext, "%d", &yylval.ival); return INT_DEC_CONST;}
{int_oct_const}     {ADJ; sscanf(yytext, "%o", &yylval.ival); return INT_OCT_CONST;}
{int_hex_const}     {ADJ; sscanf(yytext, "%x", &yylval.ival); return INT_HEX_CONST;}
{flt_dec_const}     {ADJ; sscanf(yytext, "%lf", &yylval.fval); return FLT_DEC_CONST;}
{flt_hex_const}     {ADJ; return FLT_HEX_CONST;}
{char_const}        {ADJ; yylval.cval = yytext[strlen(yytext)-2]; return CHAR_CONST;}


{str_literal}       {ADJ; cstrcpy(yytext); return STR_LITERAL;}


{lparen}            {ADJ; return LPAREN;}
{rparen}            {ADJ; return RPAREN;}
{lbracket}          {ADJ; return LBRACKET;}
{rbracket}          {ADJ; return RBRACKET;}
{lbrace}            {ADJ; return LBRACE;}
{rbrace}            {ADJ; return RBRACE;}
{lshift}            {ADJ; return LSHIFT;}
{rshift}            {ADJ; return RSHIFT;}

{dot}               {ADJ; return DOT;}
{dollar}            {ADJ; return DOLLAR;}

{arrow}             {ADJ; return ARROW;}

{double_plus}       {ADJ; return DOUBLE_PLUS;}
{double_minus}      {ADJ; return DOUBLE_MINUS;}

{bit_and}           {ADJ; return BIT_AND;}
{bit_not}           {ADJ; return BIT_NOT;}
{bit_xor}           {ADJ; return BIT_XOR;}
{bit_or}            {ADJ; return BIT_OR;}

{opt_times}         {ADJ; return OPT_TIMES;}
{opt_plus}          {ADJ; return OPT_PLUS;}
{opt_minus}         {ADJ; return OPT_MINUS;}
{opt_div}           {ADJ; return OPT_DIV;}
{opt_mod}           {ADJ; return OPT_MOD;}

{cmp_less}          {ADJ; return CMP_LESS;}
{cmp_more}          {ADJ; return CMP_MORE;}
{cmp_less_eq}       {ADJ; return CMP_LESS_EQ;}
{cmp_more_eq}       {ADJ; return CMP_MORE_EQ;}
{cmp_eq}            {ADJ; return CMP_EQ;}
{cmp_neq}           {ADJ; return CMP_NEQ;}

{log_not}           {ADJ; return LOG_NOT;}
{log_and}           {ADJ; return LOG_AND;}
{log_or}            {ADJ; return LOG_OR;}

{quest_mark}        {ADJ; return QUEST_MARK;}
{colon_mark}        {ADJ; return COLON_MARK;}
{semic_mark}        {ADJ; return SEMIC_MARK;}
{ellip_mark}        {ADJ; return ELLIP_MARK;}
{comma_mark}        {ADJ; return COMMA_MARK;}

{cal_eq}            {ADJ; return CAL_EQ;}
{cal_times_eq}      {ADJ; return CAL_TIMES_EQ;}
{cal_div_eq}        {ADJ; return CAL_DIV_EQ;}
{cal_mod_eq}        {ADJ; return CAL_MOD_EQ;}
{cal_plus_eq}       {ADJ; return CAL_PLUS_EQ;}
{cal_minus_eq}      {ADJ; return CAL_MINUS_EQ;}
{cal_left_eq}       {ADJ; return CAL_LEFT_EQ;}
{cal_right_eq}      {ADJ; return CAL_RIGHT_EQ;}
{cal_and_eq}        {ADJ; return CAL_AND_EQ;}
{cal_xor_eq}        {ADJ; return CAL_XOR_EQ;}
{cal_or_eq}         {ADJ; return CAL_OR_EQ;}

{sharp_mark}        {ADJ; return SHARP_MARK;}
{double_sharp}      {ADJ; return DOUBLE_SHARP;}



{blank_space}       {ADJ;}
{enter}             {ADJ;}
{cline}             {ADJ;}
{tab}               {ADJ;}
{default}           {ADJ; printf("error in lex: %s\n", yytext);}
%%

int yywrap(void){
    return 1;
}
