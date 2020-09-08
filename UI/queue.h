#include <stdlib.h>
#include <stdbool.h>

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

#ifndef __CUSTOM__QUEUE__
#define __CUSTOM__QUEUE__
typedef struct QNode
{
    void *data;
    struct QNode *next;
} QNode;
typedef struct Queue
{
    QNode *head;
    QNode *tail;
    int len;
} Queue;
Queue *InitQueue(void)
{
    Queue *Q = (Queue *)malloc(sizeof(Queue));
    if (!Q)
        exit(OVF);
    Q->head = NULL;
    Q->tail = NULL;
    Q->len = 0;
    return Q;
}
Status DestroyQueue(Queue *Q)
{
    if (!Q)
        return ERROR;
    if (Q->len)
    {
        QNode *p1 = Q->head;
        QNode *p2 = Q->head->next;
        while (1)
        {
            free(p1);
            p1 = p2;
            if (!p1)
                break;
            p2 = p2->next;
        }
    }
    free(Q);
    return OK;
}
Status EnQueue(Queue *Q, void *e)
{
    if (!Q)
        return ERROR;
    QNode *n = (QNode *)malloc(sizeof(QNode));
    if (!n)
        exit(OVF);
    n->data = e;
    n->next = NULL;
    if (!(Q->len))
        Q->head = n;
    else
        Q->tail->next = n;
    Q->tail = n;
    Q->len++;
    return OK;
}
void *DeQueue(Queue *Q)
{
    if (!Q || Q->len <= 0)
        return 0;
    void *e = Q->head->data;
    QNode *t = Q->head;
    Q->head = t->next;
    free(t);
    Q->len--;
    if (!(Q->len))
        Q->tail = NULL;
    return e;
}
bool InsideQueue(Queue *Q, void *e)
{
    if (!Q)
        return false;
    QNode *cur = Q->head;
    while (cur)
    {
        if (e == cur->data)
            return true;
        cur = cur->next;
    }
    return false;
}
#endif
