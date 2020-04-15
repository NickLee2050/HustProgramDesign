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
#define OVF -1
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
//Queue inner connection status
#define REMOVE 1 //Vex has arcs only to nodes within the Queue
#define WAIT -1  //Vex can possibly be UIP
#define FANOUT 0 //Fan-out
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
//Additional Data Structure
typedef struct SimpSet
{
    int *v;
    ClaNode **c;
    int len;
    int size;
} SimpSet;
#endif
//Functions
int abs(int d)
{
    return (d >= 0) ? d : (d * -1);
}
int min(int a, int b)
{
    return (a > b) ? b : a;
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
        int *n_v = (int *)realloc(SS->v, sizeof(int) * (SS->size + SIZE_INCREAMENT));
        if (!n_v)
            exit(-1);
        SS->v = n_v;
        ClaNode **n_c = (ClaNode **)realloc(SS->c, sizeof(ClaNode *) * (SS->size + SIZE_INCREAMENT));
        if (!n_c)
            exit(-1);
        SS->c = n_c;
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
int FindUnknownVar(int *solution, ClaNode *cur)
{
    if (!solution || !cur)
        return 0;
    for (int i = 0; i < cur->len; i++)
    {
        int d = cur->data[i];
        if (!(solution[abs(d)]))
            return d;
    }
    return 0;
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
int ClaSetJudge(int *solution, ClaNode *head, SimpSet *tar)
{
    ClaNode *cur = head;
    int mark = EMPTY_SET;
    int temp = 0;
    while (cur)
    {
        int valid_var = cur->len;
        for (int i = 0; i < cur->len; i++)
        {
            int d = cur->data[i];
            if (d * solution[abs(d)] > 0) //Current clause satisfied
            {
                valid_var = -1;
                break;
            }
            else if (d * solution[abs(d)] < 0) //Current variable not satisfied
                valid_var--;
        }
        //Return status & Target assign
        if (!valid_var) //Conflict
        {
            ClearSimpSet(tar);
            EnSimpSet(tar, 0, cur);
            return WITH_EMPTY_CLA;
        }
        if (valid_var != -1) //Not satisfied
        {
            if (valid_var == 1) //Unit
            {
                if (mark == SPLIT_REQD) //Overwrite
                    DeSimpSet(tar, NULL, NULL);
                temp = FindUnknownVar(solution, cur);
                if (temp)
                    EnSimpSet(tar, temp, cur);
                return WITH_SIG_CLA;
            }
            if (tar->len != 1)
            {
                temp = FindUnknownVar(solution, cur);
                if (temp)
                    EnSimpSet(tar, temp, cur);
            }
            mark = SPLIT_REQD;
        }
        cur = cur->next;
    }
    // Return a target through the pointer
    return mark;
}
Status GenResult(int *result, ALGraph *G)
{
    if (!result || !G)
        return ERROR;
    if (LocateVex(G, 0))
        return ERROR;
    for (int i = 0; i <= var_count; i++)
        result[i] = 0;
    for (int i = 0; i < G->vexnum; i++)
    {
        int d = G->vert[i]->data->key;
        result[abs(d)] = d > 0 ? 1 : -1;
    }
    return OK;
}
Status Simplify(SimpSet *target, ALGraph *G, int level, int mark)
{
    if (!G || !target)
        return ERROR;
    int var;
    ClaNode *cla;
    for (int i = 0; i < target->len; i++)
    {
        DeSimpSet(target, &var, &cla);
        InsertVex(G, var, level, 1 + G->vexnum);
        if (mark == WITH_SIG_CLA) //Arc insertion required
            for (int i = 0; i < cla->len; i++)
            {
                int d = cla->data[i];
                if (d != var)
                    InsertArc(G, var, d * -1, cla);
            }
    }
}
VNode *ConflictMark(ALGraph *G, SimpSet *tar, int level)
{
    if (!G || !tar)
        return NULL;
    ClaNode *cla;
    DeSimpSet(tar, NULL, &cla);
    VNode *V = InsertVex(G, 0, level, 1 + G->vexnum);
    for (int i = 0; i < cla->len; i++)
    {
        VNode *v = LocateVex(G, cla->data[i] * -1);
        if (v)
            InsertArc(G, 0, v->data->key, cla);
    }
    return V;
}
int CountLitAtLevel(ALGraph *G, Queue *cur, int level)
{
    if (!cur || level < 0)
        return 0;
    int count = 0;
    QNode *t = cur->head;
    while (t)
    {
        VNode *V = (VNode *)t->data;
        if (V->data->value == level)
            count++;
        t = t->next;
    }
    return count;
}
bool ResolveCond(ALGraph *G, int level, VNode *V)
{
    if (!G || !V || level < 0)
        return false;
    if (V->data->value == level && V->firstarc)
        return true;
    return false;
}
bool Resolve(ALGraph *G, VNode *tar, Queue *learnt)
{
    if (!G || !tar || !learnt)
        return false;
    ArcNode *arc = tar->firstarc;
    while (arc)
    {
        VNode *n = G->vert[arc->adjvex];
        if (!InsideQueue(learnt, n))
            EnQueue(learnt, n);
        arc = arc->nextarc;
    }
    return true;
}
bool Analyse(ALGraph *G, ClaNode **head, int *level, VNode *confv)
{
    /*
    Tasks of this function:
        1.Return false when encountering unsolvable dataset;
        2.Add a binary clause to the dataset after analysis;
        3.Calculate a level for a possibly needed backtrace;
        4.If back-trace needed, prune the implication graph.
    */
    if (!G || !head || !level)
        return false;
    //Task 1
    if (!(*level))
        return false;
    Queue *CurLearnt = InitQueue();
    Queue *result = InitQueue();
    int d = *level;
    VNode *tar = NULL;
    //Task 2
    //Initial conflict analysis
    for (ArcNode *arc = confv->firstarc; arc; arc = arc->nextarc)
        EnQueue(CurLearnt, G->vert[arc->adjvex]);
    //Analyse
    while (CurLearnt->len)
    {
        tar = (VNode *)DeQueue(CurLearnt);
        *level = min(*level, tar->data->value);
        if (ResolveCond(G, d, tar))
            Resolve(G, tar, CurLearnt);
        else if (!InsideQueue(result, tar))
            EnQueue(result, tar);
    }
    //Add on
    ClaNode *n = (ClaNode *)malloc(sizeof(ClaNode));
    if (!n)
        exit(-1);
    n->data = (int *)malloc(sizeof(int) * result->len);
    if (!(n->data))
        exit(-1);
    n->len = result->len;
    n->size = result->len;
    n->next = *head;
    *head = n;
    for (int i = 0; i < n->len; i++)
        n->data[i] = ((VNode *)DeQueue(result))->data->key * -1;
    //Task 3
    if (n->len == 1)
        *level = 0;
    //Task 4
    for (int i = 0; i < G->vexnum; i++)
    {
        if (G->vert[i]->data->value > *level)
        {
            DeleteVex(G, G->vert[i]->data->key);
            i--;
        }
    }
    DestroyQueue(result);
    DestroyQueue(CurLearnt);
    return true;
}
void Show(int *result)
{
    if (result)
        for (int i = 1; i <= var_count; i++)
        {
            if (result[i] == 1)
                printf("y ");
            if (result[i] == -1)
                printf("n ");
            if (result[i] == 0)
                printf("_ ");
            if (i % 10 == 0)
                printf("\n");
        }
    printf("\n\n");
    return;
}
int *DPLL(ClaNode *head)
{
    /*
    This DPLL algorithm applies Conflict Driven Clause Learning 
    method to provide learning and non-chronological backtrack 
    features for faster searching and to minimize memory costs.
    */
    int mark;
    int level = 0;
    ClaNode *cur = head;
    ALGraph *G = InitGraph();
    SimpSet *target = InitSimpSet();
    VNode *confv = NULL;
    int *result = (int *)malloc(sizeof(int) * (var_count + 1));
    if (!result)
        exit(-1);
    if (!cur)
        exit(0);
    while (1)
    {
        GenResult(result, G);
        mark = ClaSetJudge(result, cur, target);
        if (mark == EMPTY_SET)
            return result;
        if (mark == WITH_EMPTY_CLA)
        {
            //Show(result);
            confv = ConflictMark(G, target, level);
            if (!Analyse(G, &cur, &level, confv))
                return NULL;
            ClearSimpSet(target);
            continue;
        }
        if (mark == SPLIT_REQD)
            level++;
        Simplify(target, G, level, mark);
    }
}