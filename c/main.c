#include "absync.h"
#include "y.tab.h"
#include <stdio.h>

extern FILE *yyin;
extern A_exp root;
extern int yyparse(void);

int main(int argc, char *args[])
{
    int result;
    char s[100];
    scanf("%s", s);
    yyin = fopen(s, "r");
    result = yyparse();
    return result;
}