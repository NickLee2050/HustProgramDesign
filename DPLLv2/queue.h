#include <stdlib.h>
//Preliminaries
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
#endif
Queue *InitQueue(void)
{
    Queue *Q = (Queue *)malloc(sizeof(Queue));
    if (!Q)
        exit(OVERFLOW);
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
    QNode *new = (QNode *)malloc(sizeof(QNode));
    if (!new)
        exit(OVERFLOW);
    new->data = e;
    new->next = NULL;
    if (!Q->len)
        Q->head = new;
    else
        Q->tail->next = new;
    Q->tail = new;
    Q->len++;
    return OK;
}
void *DeQueue(Queue *Q)
{
    if (!Q || Q->len <= 0)
        return NULL;
    void *e = Q->head->data;
    QNode *t = Q->head;
    Q->head = t->next;
    free(t);
    Q->len--;
    return e;
}