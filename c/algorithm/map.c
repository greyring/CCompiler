#include <stdio.h>
#include <stdlib.h>

#define MAP_TEST

#define MAP_NORMAL    0
#define MAP_LESSTHANK 1
#define MAP_INSTACK   2

int getMinNumber(int bitnum){
    for (int i=0; i<32; i++){
        if ((1<<i)&bitnum) continue;
        return i;
    }
    return 32;
}

/**
 * int** a : interference graph
 * int   n : size of nodes
 * int   k : size of color set which must less than 32 !!!!
 * int* res: result
*/
void mapped(int** a, int n, int k, int* res){
    /*
    printf("%d %d\n", n, k);
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++){
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }
    */


    int* myStack = (int*)malloc(sizeof(int) * n);
    int nstack = 0;
    int* count = (int*)malloc(sizeof(int) * n);
    int* mark = (int*)malloc(sizeof(int) * n);
    int totsum = 0;

    for (int i=0; i<n; i++) mark[i] = MAP_NORMAL;
    for (int i=0; i<n; i++) count[i] = 0;
    for (int i=0; i<n; i++) res[i] = -1;

    //simplify
    for (int i=0; i<n; i++) for (int j=0; j<n; j++){
        count[i] += a[i][j];
    }
    for (int i=0; i<n; i++) if (count[i] < k){
        totsum += 1;
        mark[i] = MAP_LESSTHANK;
    }

    //for (int i=0; i<n; i++) printf("%d ", count[i]); printf("\n");

    //spill
    while (totsum != n){
        int mxn = -1;
        for (int i=0; i<n; i++){
            if (mark[i] != MAP_NORMAL) continue;
            if (mxn == -1 || count[mxn] < count[i]){
                mxn = i;
            }
        }
        totsum += 1;
        myStack[nstack++] = mxn;
        mark[mxn] = MAP_INSTACK;
    }

    //for (int i=0; i<n; i++) printf("%d ", mark[i]); printf("\n");

    //select
    for (int i=nstack-1; i>=0; i--){
        int index = myStack[i];
        mark[index] = MAP_NORMAL;
        int neignbor = 0;
        for (int j=0; j<n; j++){
            if (mark[j] != MAP_NORMAL) continue;
            if (a[index][j] == 0 || res[j] == -1) continue;
            neignbor |= (1 << res[j]);
        }
        int colorNum = getMinNumber(neignbor);
        //printf("id : %d %d %d %d\n", index, neignbor, colorNum, k);
        if (colorNum >= k) continue;
        res[index] = colorNum;
    }

    for (int i=0; i<n; i++){
        if (mark[i] == MAP_NORMAL) continue;
        mark[i] = MAP_NORMAL;
        int neignbor = 0;
        for (int j=0; j<n; j++){
            if (mark[j] != MAP_NORMAL) continue;
            if (a[i][j] == 0 || res[j] == -1) continue;
            neignbor |= (1 << res[j]);
        }
        int colorNum = getMinNumber(neignbor);
        if (colorNum >= k) continue;
        res[i] = colorNum;
    }


    free(myStack);
    free(mark);
    free(count);
}

#ifndef MAP_TEST

int main(){
    int n, k;
    scanf("%d%d", &n, &k);
    int **a = (int**)malloc(sizeof(int*) * n);
    for (int i=0; i<n; i++) a[i] = (int*)malloc(sizeof(int) * n);
    for (int i=0; i<n; i++) for (int j=0; j<n; j++) scanf("%d", &a[i][j]);
    /*
    a = {
        {0, 1, 1, 0, 0, 1, 0, 0, 0, 0},
        {1, 0, 1, 1, 0, 0, 1, 1, 1, 0},
        {1, 1, 0, 0, 1, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 1, 0, 1, 0, 1, 0},
        {0, 0, 1, 1, 0, 1, 1, 0, 0, 1},
        {1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
        {0, 1, 0, 1, 0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 1, 0, 1, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0}
    };
    */
    int* res = (int*)malloc(sizeof(int) * 10);;
    mapped(a, n, k, res);
    for (int i=0; i<n; i++) printf("%d ", res[i]);
    return 0;
}

#endif