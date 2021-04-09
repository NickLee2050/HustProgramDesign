#include <stdlib.h>

//Preliminaries
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

#ifndef __CUSTOM__GRAPH__
#define __CUSTOM__GRAPH__
//Data Structure Design
typedef struct VertexType
{
    int key;
    int value;
} VertexType;
typedef struct ArcNode
{
    int adjvex;
    void *info;
    struct ArcNode *nextarc;
} ArcNode;
typedef struct VNode
{
    VertexType *data;
    ArcNode *firstarc;
    int info;
} VNode;
typedef struct ALGraph
{
    VNode **vert;
    int vexnum, arcnum;
    int size;
} ALGraph;
#define MAX_VEX_NUM 100
//Functions
ALGraph *InitGraph()
{
    ALGraph *G = (ALGraph *)malloc(sizeof(ALGraph));
    if (!G)
        exit(OVF);
    G->vert = (VNode **)malloc(sizeof(VNode *) * MAX_VEX_NUM);
    if (!(G->vert))
        exit(OVF);
    for (int i = 0; i < MAX_VEX_NUM; i++)
        G->vert[i] = NULL;
    G->arcnum = 0;
    G->vexnum = 0;
    G->size = MAX_VEX_NUM;
    return G;
}
Status DestroyGraph(ALGraph *G)
{
    if (!G) //Validation
        return ERROR;
    for (int i = 0; i < G->vexnum; i++)
    {
        ArcNode *p1 = G->vert[i]->firstarc;
        ArcNode *p2 = NULL;
        while (p1)
        {
            p2 = p1->nextarc;
            free(p1);
            p1 = p2;
            free(G->vert[i]);
        }
        free(G);
    }
    return OK;
}
VNode *LocateVex(ALGraph *G, int key)
{
    if (!G)
        return NULL;
    for (int i = 0; i < G->vexnum; i++)
    {
        if (key == G->vert[i]->data->key)
            return G->vert[i];
    }
    return NULL;
}
Status PutVex(ALGraph *G, int key, int value)
{
    if (!G)
        return ERROR;
    VNode *p = LocateVex(G, key);
    if (!p)
        return ERROR;
    p->data->value = value;
    return OK;
}
int FirstAdjVex(ALGraph *G, int pos)
{
    if (!G || pos >= G->vexnum)
        return INFEASIBLE;
    if (!G->vert[pos]->firstarc)
        return INFEASIBLE;
    return G->vert[pos]->firstarc->adjvex;
}
int NextAdjVex(ALGraph *G, int vpos, int wpos)
{
    if (!G || vpos >= G->vexnum || wpos >= G->vexnum)
        return INFEASIBLE;
    ArcNode *p = G->vert[vpos]->firstarc;
    while (p)
    {
        if (p->adjvex == wpos)
        {
            p = p->nextarc;
            break;
        }
        p = p->nextarc;
    }
    if (p == NULL)
        return INFEASIBLE;
    return p->adjvex;
}
VNode *InsertVex(ALGraph *G, int key, int value, int info)
{
    if (!G)
        return NULL;
    for (int i = 0; i < G->vexnum; i++) //Repeat detection
    {
        if (key == G->vert[i]->data->key)
            return NULL;
    }
    if (G->vexnum == G->size) //Resize
    {
        int prev = G->size;
        VNode **n = (VNode **)realloc(G->vert, sizeof(VNode *) * (G->size + SIZE_INCREAMENT));
        if (!n)
            exit(OVF);
        G->vert = n;
        G->size += SIZE_INCREAMENT;
        for (int i = prev; i < G->size; i++)
            G->vert[i] = NULL;
    }
    //Initialization
    VNode *V = (VNode *)malloc(sizeof(VNode));
    if (!V)
        exit(OVF);
    V->data = (VertexType *)malloc(sizeof(VertexType));
    if (!(V->data))
        exit(OVF);
    V->data->key = key;
    V->data->value = value;
    V->info = info;
    V->firstarc = NULL;
    //Assign
    G->vert[G->vexnum] = V;
    (G->vexnum)++;
    return V;
}
Status DeleteVex(ALGraph *G, int key)
{
    if (!G)
        return ERROR;
    VNode *p = LocateVex(G, key);
    if (!p)
        return ERROR;
    int count = 0;
    while (p != G->vert[count])
        count++;
    for (int i = 0; i < G->vexnum; i++)
    {
        if (i == count)
            continue;
        ArcNode *t1 = G->vert[i]->firstarc;
        if (!t1)
            continue;
        if (t1->adjvex == count)
        {
            G->vert[i]->firstarc = t1->nextarc;
            free(t1);
            (G->arcnum)--;
        } //special case
        else
        {
            ArcNode *t2 = t1; //t2 points to the previous arc of t1
            t1 = t1->nextarc;
            while (t1)
            {
                if (t1->adjvex == count)
                {
                    t2->nextarc = t1->nextarc; //reconstruct links
                    free(t1);
                    (G->arcnum)--;
                    break; //found
                }
                t2 = t1;
                t1 = t1->nextarc;
            } //end without break, meaning not found
        }
    } //free arcs to vex
    ArcNode *p1 = p->firstarc;
    ArcNode *p2 = NULL;
    while (p1)
    {
        p2 = p1->nextarc;
        free(p1);
        (G->arcnum)--;
        p1 = p2;
    }
    free(p); //free arcs from vex
    for (int j = 0; j < G->vexnum; j++)
    {
        if (j == count)
            continue;
        ArcNode *t = G->vert[j]->firstarc;
        while (t)
        {
            if (t->adjvex > count)
                (t->adjvex)--;
            t = t->nextarc;
        }
    } //redirect before sorting
    for (int k = count; k < G->vexnum; k++)
        G->vert[k] = G->vert[k + 1]; //sorting
    (G->vexnum)--;
    return OK;
}
Status InsertArc(ALGraph *G, int vkey, int wkey, void *info)
{
    if (!G)
        return ERROR;
    VNode *v = LocateVex(G, vkey);
    VNode *w = LocateVex(G, wkey);
    if (!v || !w)
        return ERROR;
    int vcount = 0, wcount = 0;
    while (v != G->vert[vcount])
        vcount++;
    while (w != G->vert[wcount])
        wcount++;
    ArcNode *t = v->firstarc;
    while (t)
    {
        if (t->adjvex == wcount) //arc existing
            return ERROR;
        t = t->nextarc;
    }
    //Initialization
    ArcNode *n = (ArcNode *)malloc(sizeof(ArcNode));
    if (!n)
        exit(OVF);
    (G->arcnum)++;
    n->adjvex = wcount;
    n->nextarc = NULL;
    n->info = info;
    if (v->firstarc == NULL) //Special case
        v->firstarc = n;
    else
    {
        t = v->firstarc;
        while (t->nextarc)
            t = t->nextarc;
        t->nextarc = n;
    }
    return OK;
}
Status DeleteArc(ALGraph *G, int vkey, int wkey)
{
    if (!G)
        return ERROR;
    VNode *v = LocateVex(G, vkey);
    VNode *w = LocateVex(G, wkey);
    if (!v || !w)
        return ERROR;
    int vcount = 0, wcount = 0;
    while (v != G->vert[vcount])
        vcount++;
    while (w != G->vert[wcount])
        wcount++;
    ArcNode *t1 = v->firstarc;
    if (!t1)
        return ERROR;
    if (t1->adjvex == wcount)
    {
        v->firstarc = t1->nextarc;
        free(t1);
        (G->arcnum)--;
    } //special case
    else
    {
        ArcNode *t2 = t1; //t2 points to the previous arc of t1
        t1 = t1->nextarc;
        while (t1)
        {
            if (t1->adjvex == wcount)
            {
                t2->nextarc = t1->nextarc; //reconstruct links
                free(t1);
                (G->arcnum)--;
                break; //found
            }
            t2 = t1;
            t1 = t1->nextarc;
        } //end without break, meaning not found
    }
    return OK;
}
#endif
