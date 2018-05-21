#include "absync.h"
#include "testAST.h"
#include "y.tab.h"
#include <stdio.h>

extern FILE *yyin;
extern A_def root;
extern int yyparse(void);

int main(int argc, char *args[])
{
    int result;
    char s[100];
    scanf("%s", s);
    yyin = fopen(s, "r");
    if (!yyin)
    {
        printf("File open failed\n");
        return -1;
    }
    result = yyparse();
    if (result)
    {
        printf("parse failed\n");
        return -1;
    }
    TA_def(root);
    printf("%s\n", TA_getRes());
    return result;
}