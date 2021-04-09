#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "DPLLv2.h"
int main()
{
    FILE *DSR = fopen("./eg/sat/L/eh-dp04s04.shuffled-1075.cnf", "r+");
    if (!DSR)
        exit(-1);
    ClaNode *head = CNF_Read(DSR);
    SearchNode *root = InitSearchNode();
    clock_t start = clock();
    SearchNode *result = DPLL(root, head);
    clock_t end = clock();
    double period = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    if (result)
        printf("Satisfied.\n");
    else
        printf("Not satisfied.\n");
    printf("Time Cost: %.1f ms\n", period);
    return 0;
}