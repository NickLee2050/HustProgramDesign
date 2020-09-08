#include <stdio.h>
#include <stdlib.h>
#ifndef __PRELIM__
#define __PRELIM__
#define TRUE 1
#define OK 1
#define FALSE 0
#define ERROR 0
#define INFEASIBLE -1
#define OVF -1
#define INIT_SIZE 10
#define SIZE_INCREAMENT 5
typedef int Status;
#endif
#ifndef __DS__DEF__
#define __DS__DEF__
#define EMPTY_SET 2
#define WITH_SIG_CLA 1
#define SPLIT_REQD 0
#define WITH_EMPTY_CLA -1
/*
Definition explanation:
    EMPTY_SET:
        The set is empty.
        return true in DPLL next.
    WITH_SIG_CLA:
        The set contains single clause(s).
        Do simplification next.
    SPLIT_REQD:
        The set contains neither single nor empty clauses.
        Do split operation next.
    WITH_EMPTY_CLA:
        The set contains empty clause(s).
        return false in DPLL next.
*/
static int var_count, cla_count;
//Data Structure Design
typedef struct ClauseNode
{
    int *data;
    int len;
    int size;
    struct ClauseNode *next;
} ClaNode;
//Additional Data Structure
typedef struct SimpSet
{
    int *v;
    ClaNode **c;
    int len;
    int size;
} SimpSet;
ClaNode *InitCla()
{
    ClaNode *cur = (ClaNode *)malloc(sizeof(ClaNode));
    if (!cur)
        exit(-1);
    cur->data = (int *)malloc(sizeof(int) * INIT_SIZE);
    if (!(cur->data))
        exit(-1);
    cur->size = INIT_SIZE;
    cur->next = NULL;
    cur->len = 0;
    return cur;
}
Status AddToCla(ClaNode *cur, int d)
{
    if (!cur)
        return ERROR;
    cur->len += 1;
    if (cur->len > cur->size)
    {
        int *n = (int *)realloc(cur->data, sizeof(int) * (cur->size + SIZE_INCREAMENT));
        if (!n)
            exit(-1);
        cur->data = n;
        cur->size += SIZE_INCREAMENT;
    } //Resize
    cur->data[cur->len - 1] = d;
    return OK;
}
Status DeleteFromCla(ClaNode *cur, int d)
{
    if (!cur)
        return ERROR;
    for (int i = 0; i < cur->len; i++)
    {
        if (cur->data[i] == d)
        {
            for (int j = i; j < cur->len - 1; j++)
                cur->data[j] = cur->data[j + 1];
            cur->len--;
            return OK;
        }
    }
    return ERROR;
}
Status DestroyCla(ClaNode *cur)
{
    if (!cur)
        return ERROR;
    if (cur->data)
        free(cur->data);
    free(cur);
    return OK;
}
ClaNode *CopyClaSet(ClaNode *head)
{
    if (!head)
        return NULL;
    ClaNode *first = InitCla();
    ClaNode *p_n = first;
    ClaNode *p = head;
    while (p)
    {
        for (int i = 0; i < p->len; i++)
            AddToCla(p_n, p->data[i]);
        if (p->next)
            p_n->next = InitCla();
        p = p->next;
        p_n = p_n->next;
    }
    return first;
}
/*
void Print_CNF(ClaNode *head)
{
    if (!head)
        return;
    ClaNode *cur = head;
    while (cur)
    {
        for (int i = 0; i < cur->len; i++)
            printf("%d ", cur->data[i]);
        printf("0\n");
        cur = cur->next;
    }
    return;
}
*/
ClaNode *CNF_Read(FILE *DSR)
{
    //Variables
    int d;
    char c = fgetc(DSR);
    char *line = (char *)malloc(sizeof(char) * 1024);
    ClaNode *head = InitCla();
    ClaNode *cur = head;
    ClaNode *prev = NULL;
    //Skip notes in the *.cnf file
    while (c == 'c')
    {
        fgets(line, 1024, DSR);
        c = fgetc(DSR);
    }
    free(line);
    //First line reading
    fscanf(DSR, " cnf %d %d", &var_count, &cla_count);
    if (!var_count || !cla_count)
        return NULL;
    //Other lines reading
    while (fscanf(DSR, "%d", &d) != EOF)
    {
        if (!d)
        {
            cur->next = InitCla();
            prev = cur;
            cur = cur->next;
            continue;
        }
        AddToCla(cur, d);
    }
    //Deal with the empty tail
    DestroyCla(cur);
    if (!prev)
        return NULL;
    prev->next = NULL;
    return head;
}
void CNF_Save(ClaNode *head, FILE *DSS)
{
    if (!head || !DSS)
        return;
    ClaNode *cur = head;
    fprintf(DSS, "c \tThis Binary Puzzle is generated by\nc \tthe algorithm written by Shuhan Li\n");
    fprintf(DSS, "p cnf %d %d\n", var_count, cla_count);
    while (cur)
    {
        for (int i = 0; i < cur->len; i++)
            fprintf(DSS, "%d ", cur->data[i]);
        fprintf(DSS, "0\n");
        cur = cur->next;
    }
    return;
}
void RES_Write(FILE *DSW, ClaNode *head, int *(*DPLL)(ClaNode *))
{
    if (!DSW)
        return;
    clock_t start = clock();
    int *result = DPLL(head);
    clock_t end = clock();
    double period = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    if (!result)
        fprintf(DSW, "s 0\nt %.0f\n", period);
    else
    {
        fprintf(DSW, "s 1\nv ");
        for (int i = 1; i <= var_count; i++)
        {
            int d = i * result[i];
            if (d)
                fprintf(DSW, "%d ", d);
        }
        fprintf(DSW, "\nt %.0f\n", period);
    }
    return;
}
SimpSet *InitSimpSet()
{
    SimpSet *SS = (SimpSet *)malloc(sizeof(SimpSet));
    if (!SS)
        exit(-1);
    SS->v = (int *)malloc(sizeof(int) * INIT_SIZE);
    if (!(SS->v))
        exit(-1);
    SS->c = (ClaNode **)malloc(sizeof(ClaNode *) * INIT_SIZE);
    if (!(SS->c))
        exit(-1);
    SS->len = 0;
    SS->size = INIT_SIZE;
    return SS;
}
Status DestorySimpSet(SimpSet *SS)
{
    if (!SS)
        return ERROR;
    if (SS->v)
        free(SS->v);
    if (SS->c)
        free(SS->c);
    free(SS);
    return OK;
}
Status ClearSimpSet(SimpSet *SS)
{
    if (!SS)
        return ERROR;
    SS->len = 0;
    return OK;
}
Status EnSimpSet(SimpSet *SS, int var, ClaNode *cla)
{
    if (!SS)
        return ERROR;
    if (SS->len == SS->size)
    {
        int *new_v = (int *)realloc(SS->v, sizeof(int) * (SS->size + SIZE_INCREAMENT));
        if (!new_v)
            exit(-1);
        SS->v = new_v;
        ClaNode **new_c = (ClaNode **)realloc(SS->c, sizeof(ClaNode *) * (SS->size + SIZE_INCREAMENT));
        if (!new_c)
            exit(-1);
        SS->c = new_c;
        SS->size += SIZE_INCREAMENT;
    }
    SS->v[SS->len] = var;
    SS->c[SS->len] = cla;
    SS->len++;
    return OK;
}
Status DeSimpSet(SimpSet *SS, int *var, ClaNode **cla)
{
    if (!SS || !(SS->len))
        return ERROR;
    if (var)
        *var = SS->v[SS->len - 1];
    if (cla)
        *cla = SS->c[SS->len - 1];
    SS->len--;
    return OK;
}
#endif