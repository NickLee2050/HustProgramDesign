#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"
#include "queue.h"
#include "graph.h"
#include "DataSetDef.h"
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
#ifndef __DPLL__
#define __DPLL__
//Functions
int abs(int d)
{
    return (d >= 0) ? d : (d * -1);
}
int min(int a, int b)
{
    return (a > b) ? b : a;
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
        InsertVex(G, var, level, G->vexnum + 1);
        if (mark == WITH_SIG_CLA) //Arc insertion required
            for (int i = 0; i < cla->len; i++)
            {
                int d = cla->data[i];
                if (d != var)
                {
                    InsertArc(G, var, d * -1, cla);
                }
            }
    }
    return OK;
}
VNode *ConflictMark(ALGraph *G, SimpSet *tar, int level)
{
    if (!G || !tar)
        return NULL;
    ClaNode *cla;
    DeSimpSet(tar, NULL, &cla);
    VNode *V = InsertVex(G, 0, level, G->vexnum + 1);
    for (int i = 0; i < cla->len; i++)
    {
        VNode *v = LocateVex(G, cla->data[i] * -1);
        if (v)
            InsertArc(G, 0, v->data->key, cla);
    }
    return V;
}
void *GetLatest(Queue *Q)
{
    if (!Q || !(Q->len))
        return NULL;
    if (Q->len == 1)
        return DeQueue(Q);
    QNode *prev = NULL;
    QNode *cur = Q->head;
    int d = ((VNode *)cur->data)->info;
    QNode *mark = Q->head;
    while (cur->next)
    {
        int order = ((VNode *)cur->next->data)->info;
        if (order > d)
        {
            prev = cur;
            mark = cur->next;
            d = order;
        }
        cur = cur->next;
    }
    if (Q->head == mark)
        Q->head = Q->head->next;
    else if (Q->tail == mark)
    {
        Q->tail = prev;
        Q->tail->next = NULL;
    }
    else
        prev->next = mark->next;
    void *V = mark->data;
    free(mark);
    Q->len--;
    return V;
}
int CountLitAtLevel(Queue *cur, int level)
{
    if (!cur || level < 0 || !(cur->len))
        return 0;
    int count = 0;
    QNode *t = cur->head;
    while (t)
    {
        VNode *V = (VNode *)(t->data);
        if (V->data->value == level)
            count++;
        t = t->next;
    }
    return count;
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
    VNode *tar = NULL;
    //Task 2
    //Initial conflict analysis
    for (ArcNode *arc = confv->firstarc; arc; arc = arc->nextarc)
        EnQueue(CurLearnt, G->vert[arc->adjvex]);
    //Analyse
    while (CountLitAtLevel(CurLearnt, *level) + CountLitAtLevel(result, *level) != 1)
    {
        tar = (VNode *)GetLatest(CurLearnt);
        if (tar->data->value != *level || !(tar->firstarc))
        {
            if (!InsideQueue(result, tar))
                EnQueue(result, tar);
        }
        else
            Resolve(G, tar, CurLearnt);
    }
    while (CurLearnt->len)
        EnQueue(result, DeQueue(CurLearnt));
    //Add on
    ClaNode *n = InitCla();
    n->next = *head;
    *head = n;
    for (int i = 0, len = result->len; i < len; i++)
    {
        VNode *v = (VNode *)DeQueue(result);
        //Task 3
        *level = min(*level, v->data->value);
        AddToCla(n, v->data->key * -1);
    }
    //Task 3 Special case
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
    DestroyQueue(CurLearnt);
    DestroyQueue(result);
    return true;
}
void PrintResult(int *result, int size)
{
    if (result)
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                if (result[i * size + j + 1] == 1)
                    printf("y ");
                else if (result[i * size + j + 1] == -1)
                    printf("n ");
                else if (result[i * size + j + 1] == 0)
                    printf("_ ");
            }
            printf("\n");
        }
    printf("\n");
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
    GenResult(result, G);
    while (1)
    {
        mark = ClaSetJudge(result, cur, target);
        if (mark == EMPTY_SET)
            return result;
        if (mark == WITH_EMPTY_CLA)
        {
            confv = ConflictMark(G, target, level);
            if (!Analyse(G, &cur, &level, confv))
                return NULL;
            GenResult(result, G);
            ClearSimpSet(target);
            continue;
        }
        if (mark == SPLIT_REQD)
            level++;
        int n = target->v[0];
        result[abs(n)] = n > 0 ? 1 : -1;
        Simplify(target, G, level, mark);
    }
}
#endif
