#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "DPLLv3.3.h"
#include "GenBoard.h"
#include "ConvertBoard.h"
int main()
{
    int size = 12;
    clock_t start = clock();
    board *b = GenBoard(size, 0.5);
    clock_t end = clock();
    double period = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    printf("Generate Time Cost: %.1f ms\n\n", period);
    ClaNode *head = ConvertBoard(b);
    ClaNode *n = CopyClaSet(head);
    start = clock();
    int *result = DPLL(n);
    end = clock();
    period = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    printf("DPLL Time Cost: %.1f ms\n", period);
    if (result)
        PrintResult(result, size);
    start = clock();
    board *solu = NaiveSolve(b);
    end = clock();
    period = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    printf("Naive Solve Time Cost: %.1f ms\n", period);
    PrintBoard(solu);
    /*
    FILE *DSR = fopen("./eg/sat/L/eh-dp04s04.shuffled-1075.cnf", "r+");
    if (!DSR)
        exit(-1);
    ClaNode *head = CNF_Read(DSR);
    clock_t start = clock();
    int *result = DPLL(head);
    clock_t end = clock();
    double period = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    printf("Time Cost: %.1f ms\n", period);
    */
    return 0;
}