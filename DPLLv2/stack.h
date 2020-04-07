#include <stdlib.h>
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
#ifndef __CUSTOM__STACK__
#define __CUSTOM__STACK__
typedef struct Stack
{
    void **data;
    int top;
    int size;
} Stack;
#endif
//Data Structure Design
//Functions
Stack *InitStack(void)
{
    Stack *S = (Stack *)malloc(sizeof(Stack));
    if (!S)
        exit(OVERFLOW);
    S->data = (void **)malloc(sizeof(void *) * INIT_SIZE);
    if (!S->data)
        exit(OVERFLOW);
    S->top = -1;
    S->size = INIT_SIZE;
    return S;
}
Status Push(Stack *S, void *e)
{
    if (!S)
        return ERROR;
    if (S->top + 1 == S->size)
    {
        void **new = (void **)realloc(S->data, sizeof(void *) * (S->size + SIZE_INCREAMENT));
        if (!new)
            exit(OVERFLOW);
        S->data = new;
        S->size += SIZE_INCREAMENT;
    }
    S->data[++(S->top)] = e;
    return OK;
}
void *Pop(Stack *S)
{
    if (!S || S->top < 0)
        return NULL;
    return S->data[(S->top)--];
}