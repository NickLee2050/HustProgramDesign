#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
#include "queue.h"
#include "graph.h"
#ifndef __PRELIM__
#define __PRELIM__
#define TRUE 1
#define OK 1
#define FALSE 0
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -1
#define INIT_SIZE 10
#define SIZE_INCREAMENT 5
typedef int Status;
#endif
#ifndef __DPLL__STATUS__
#define __DPLL__STATUS__
#define EMPTY_SET 2
#define WITH_SIG_CLA 1
#define SPLIT_REQD 0
#define WITH_EMPTY_CLA -1
static int var_count, cla_count;
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
//Data Structure Design
typedef struct ClauseNode
{
    int *data;
    int len;
    int size;
    struct ClauseNode *next;
} ClaNode;
typedef struct SearchNode
{
    struct SearchNode *prev;
    int *solution;
    struct SearchNode *lchild;
    struct SearchNode *rchild;
    int lmark;
    int rmark;
} SearchNode;
//Additional Data Structure
typedef struct SimpSet
{
    int *data;
    int len;
    int size;
} SimpSet;
#endif
//Functions
int abs(int d)
{
    return (d >= 0) ? d : (d * -1);
}
SimpSet *InitSimpSet()
{
    SimpSet *SS = (SimpSet *)malloc(sizeof(SimpSet));
    if (!SS)
        exit(-1);
    SS->data = (int *)malloc(sizeof(int) * INIT_SIZE);
    if (!(SS->data))
        exit(-1);
    SS->len = 0;
    SS->size = INIT_SIZE;
    return SS;
}
Status DestorySimpSet(SimpSet *SS)
{
    if (!SS)
        return ERROR;
    if (SS->data)
        free(SS->data);
    free(SS);
    return OK;
}
Status ClearSimpSet(SimpSet *SS)
{
    if (!SS)
        return ERROR;
    SS->len = 0;
}
Status EnSimpSet(SimpSet *SS, int var)
{
    if (!SS)
        return ERROR;
    if (SS->len == SS->size)
    {
        int *n = (int *)realloc(SS->data, sizeof(int) * (SS->size + SIZE_INCREAMENT));
        if (!n)
            exit(-1);
        SS->data = n;
        SS->size += SIZE_INCREAMENT;
    }
    SS->data[SS->len] = var;
    SS->len++;
    return OK;
}
int DeSimpSet(SimpSet *SS)
{
    if (!SS || !(SS->len))
        return 0;
    int t = SS->data[--(SS->len)];
    return t;
}
SearchNode *InitSearchNode(void)
{
    SearchNode *n = (SearchNode *)malloc(sizeof(SearchNode));
    if (!n)
        exit(-1);
    n->prev = NULL;
    n->solution = (int *)malloc(sizeof(int) * (var_count + 1));
    for (int i = 0; i <= var_count; i++)
        n->solution[i] = 0;
    n->lchild = NULL;
    n->rchild = NULL;
    n->lmark = 0;
    n->rmark = 0;
    return n;
}
ClaNode *CNF_Read(FILE *DSR)
{
    //Variables
    int d;
    char c = fgetc(DSR);
    char *line = (char *)malloc(sizeof(char) * 1024);
    ClaNode *head = (ClaNode *)malloc(sizeof(ClaNode));
    if (!head)
        exit(-1);
    ClaNode *cur = head;
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
    fscanf(DSR, "%d", &d);
    while (1)
    {
        //Initialization
        cur->data = (int *)malloc(sizeof(int) * INIT_SIZE);
        if (!(cur->data))
            exit(-1);
        cur->size = INIT_SIZE;
        cur->next = NULL;
        cur->data[0] = d;
        cur->len = 1;
        while (fscanf(DSR, "%d", &d), d != 0)
        {
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
        }
        //Preparation for the next cycle
        if (fscanf(DSR, "%d", &d) != EOF)
        {
            cur->next = (ClaNode *)malloc(sizeof(ClaNode));
            if (!(cur->next))
                exit(-1);
            cur = cur->next;
        }
        //Cycle ending
        else
            break;
    }
    return head;
}
int *Copy(int *solution)
{
    if (!solution)
        return NULL;
    int *n = (int *)malloc(sizeof(int) * var_count + 1);
    if (!n)
        exit(-1);
    for (int i = 0; i <= var_count; i++)
        n[i] = solution[i];
    return n;
}
int FindSingleCla(SearchNode *root, ClaNode *head)
{
    ClaNode *cur = head;
    while (cur)
    {
        int valid_var = cur->len;
        for (int i = 0; i < cur->len; i++)
        {
            int d = cur->data[i];
            if (d * root->solution[abs(d)] > 0)
                break;
            else if (root->solution[abs(d)] < 0)
                valid_var--;
        }
        if (valid_var == 1)
            cur = cur->next;
    }
}
SearchNode *NewBranch(int v, SearchNode *root)
{
    SearchNode *n = (SearchNode *)malloc(sizeof(SearchNode));
    if (!n)
        exit(-1);
    n->prev = root;
    n->solution = Copy(root->solution);
    n->lchild = NULL;
    n->rchild = NULL;
    n->lmark = 0;
    n->rmark = 0;
    if (v > 0)
        n->solution[v] = 1; //positive
    else
        n->solution[v * -1] = -1; //negative
    return n;
}
int ChooseVarByLogistics(SearchNode *root, ClaNode *head)
{
    int *logist = (int *)malloc(sizeof(int) * (var_count + 1));
    for (int i = 0; i <= var_count; i++)
        logist[i] = 0;
    ClaNode *cur = head;
    while (cur)
    {
        int mark = 0;
        for (int i = 0; i < cur->len; i++)
        {
            int d = cur->data[i];
            if (d * root->solution[abs(d)] > 0) //Current clause satisfied
            {
                mark = 1;
                break;
            }
        }
        if (!mark)
        {
            for (int i = 0; i < cur->len; i++)
            {
                int d = cur->data[i];
                if (root->solution[abs(d)] == 0) //Value not assigned for current d
                    logist[abs(d)] += 1;
            }
        }
        cur = cur->next;
    }
    int max = 1;
    for (int i = 1; i <= var_count; i++)
        if (logist[i] > max)
            max = i;
    return max;
}
SearchNode *DeleteSearchNode(SearchNode *root)
{
    if (!root)
        return NULL;
    SearchNode *prev = root->prev;
    free(root->solution);
    free(root);
    return prev;
}
int ClaSetJudge(SearchNode *root, ClaNode *head, SimpSet *tar)
{
    ClaNode *cur = head;
    int mark = EMPTY_SET;
    Queue *Q = InitQueue(); //Store single clause(s)
    while (cur)
    {
        int valid_var = cur->len;
        for (int i = 0; i < cur->len; i++)
        {
            int d = cur->data[i];
            if (d * root->solution[abs(d)] > 0) //Current clause satisfied
            {
                valid_var = -1;
                break;
            }
            else if (d * root->solution[abs(d)] < 0) //Current variable not satisfied
                valid_var--;
        }
        //Return status & Target assign
        if (!valid_var)
            return WITH_EMPTY_CLA;
        else if (valid_var == 1)
        {
            if (mark == SPLIT_REQD)
                DeQueue(Q); //Overwrite previous target
            EnQueue(Q, cur);
            mark = WITH_SIG_CLA;
        }
        //Single clause not found && current clause not satisfied
        else if (mark != WITH_SIG_CLA && valid_var != -1)
        {
            if (Q->len != 1)
                EnQueue(Q, cur); //Reserve a target for further split
            mark = SPLIT_REQD;
        }
        cur = cur->next;
    }
    //SimpSet preparation
    while (Q->len)
    {
        cur = (ClaNode *)DeQueue(Q);
        for (int i = 0; i < cur->len; i++)
        {
            int d = cur->data[i];
            if (!(root->solution[abs(d)]))
            {
                EnSimpSet(tar, d);
                break;
            }
        }
    }
    DestroyQueue(Q);
    // Return a target through the pointer
    return mark;
}
void Simplify(SearchNode *root, SimpSet *tar)
{
    if (root && tar)
    {
        for (int i = 0; i < tar->len; i++)
        {
            int t = tar->data[i];
            if (t > 0)
                root->solution[t] = 1;
            else if (t < 0)
                root->solution[t * -1] = -1;
        }
    }
    ClearSimpSet(tar);
    return;
}
SearchNode *DPLL(SearchNode *root, ClaNode *head)
{
    //This DPLL algorithm applies advanced deletion and
    //non-recursive method with linear storage structure
    //to improve the performance of simplification and branching
    int count = 0;
    int mark;
    int var = 0;
    Stack *S = InitStack();
    ALGraph *G = InitGraph();
    SimpSet *target = InitSimpSet();
    SearchNode *cur = NULL;
    Push(S, root);
    while (S->top >= 0)
    {
        int tar = 0;
        cur = (SearchNode *)Pop(S);
        if (!cur)
            return NULL;
        if (cur->lmark && cur->rmark)
        {
            Push(S, cur->prev);
            continue;
        }
        while (1) //Simplification when single clause(s) exist
        {
            mark = ClaSetJudge(cur, head, target);
            if (mark == WITH_SIG_CLA)
                Simplify(cur, target);
            else if (mark == EMPTY_SET)
                return cur;
            else
                break;
        }
        var = DeSimpSet(target);
        if (mark != WITH_EMPTY_CLA)
        {
            if (cur->lchild == NULL)
            {
                cur->lchild = NewBranch(var, cur);
                cur->lmark = 1;
                Push(S, cur->lchild);
            }
            else if (cur->rchild == NULL)
            {
                cur->rchild = NewBranch(var * -1, cur);
                cur->rmark = 1;
                Push(S, cur->rchild);
            }
        }
        else
            Push(S, cur->prev); //Return to the previous node
    }
}