#include "absync.h"
#include "testAST.h"
#include "y.tab.h"
#include <stdio.h>

extern FILE *yyin;
extern A_stat root;
extern int yyparse(void);

int main(int argc, char *args[])
{
    int result;
    char s[100];
    scanf("%s", s);
    yyin = fopen(s, "r");
    result = yyparse();
    TA_stat(root);
    printf("%s\n", TA_getRes());
    return result;
}