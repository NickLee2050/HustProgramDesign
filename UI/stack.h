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
        exit(OVF);
    S->data = (void **)malloc(sizeof(void *) * INIT_SIZE);
    if (!S->data)
        exit(OVF);
    S->top = -1;
    S->size = INIT_SIZE;
    return S;
}
Status DestroyStack(Stack *S)
{
    if (!S)
        return ERROR;
    if (S->data)
        free(S->data);
    free(S);
    return OK;
}
Status Push(Stack *S, void *e)
{
    if (!S)
        return ERROR;
    if (S->top + 1 == S->size)
    {
        void **n = (void **)realloc(S->data, sizeof(void *) * (S->size + SIZE_INCREAMENT));
        if (!n)
            exit(OVF);
        S->data = n;
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
bool EmptyStack(Stack *S)
{
    if (!S || S->top == -1)
        return true;
    return false;
}
#endif
