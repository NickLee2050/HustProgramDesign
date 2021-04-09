#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "stack.h"
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

#ifndef __BOARD__GEN__
#define __BOARD__GEN__
typedef struct board
{
    int **data;
    int size;
} board;
typedef struct pair
{
    int x;
    int y;
    int info;
} pair;
#define RAND_RATIO 0.4
void PrintBoard(board *b)
{
    if (b)
        for (int i = 0; i < b->size; i++)
        {
            for (int j = 0; j < b->size; j++)
            {
                if (b->data[i][j] == 1)
                    printf("y ");
                else if (b->data[i][j] == -1)
                    printf("n ");
                else if (b->data[i][j] == 0)
                    printf("_ ");
            }
            printf("\n");
        }
    printf("\n");
    return;
}
board *InitBoard(int size)
{
    board *b = (board *)malloc(sizeof(board));
    if (!b)
        exit(-1);
    b->size = size;
    b->data = (int **)malloc(sizeof(int *) * size);
    if (!b->data)
        exit(-1);
    for (int i = 0; i < size; i++)
    {
        b->data[i] = (int *)malloc(sizeof(int) * size);
        if (!(b->data[i]))
            exit(-1);
    }
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            b->data[i][j] = 0;
    return b;
}
Status DestroyBoard(board *b)
{
    if (!b || !b->data)
        return ERROR;
    for (int i = 0; i < b->size; i++)
        free(b->data[i]);
    free(b->data);
    free(b);
    return OK;
}
board *CopyBoard(board *b)
{
    board *n = (board *)malloc(sizeof(board));
    if (!n)
        exit(-1);
    n->size = b->size;
    n->data = (int **)malloc(sizeof(int *) * b->size);
    if (!n->data)
        exit(-1);
    for (int i = 0; i < n->size; i++)
    {
        n->data[i] = (int *)malloc(sizeof(int) * n->size);
        if (!(n->data[i]))
            exit(-1);
    }
    for (int i = 0; i < n->size; i++)
        for (int j = 0; j < n->size; j++)
            n->data[i][j] = b->data[i][j];
    return n;
}
pair *InitPair()
{
    pair *p = (pair *)malloc(sizeof(pair));
    p->x = 0;
    p->y = 0;
    p->info = 0;
    if (!p)
        exit(-1);
    return p;
}
Status DestroyPair(pair *p)
{
    if (!p)
        return ERROR;
    free(p);
    return OK;
}
bool TripletDetect(board *b)
{
    if (!b)
        return false;
    //Verify that there is no triplet
    //In row
    for (int i = 0; i < b->size; i++)
        for (int j = 0; j < b->size - 2; j++)
            if (b->data[i][j])
                if (b->data[i][j] == b->data[i][j + 1] &&
                    b->data[i][j + 1] == b->data[i][j + 2])
                    return true;
    //In line
    for (int i = 0; i < b->size; i++)
        for (int j = 0; j < b->size - 2; j++)
            if (b->data[j][i])
                if (b->data[j][i] == b->data[j + 1][i] &&
                    b->data[j + 1][i] == b->data[j + 2][i])
                    return true;
    return false;
}
bool ValidBoard(board *b)
{
    if (!b)
        return false;
    //Verify the quantity of ones and zeros
    //In Rows
    for (int i = 0; i < b->size; i++)
    {
        int pos = 0, neg = 0;
        for (int j = 0; j < b->size; j++)
        {
            if (b->data[i][j] == 1)
                pos++;
            else if (b->data[i][j] == -1)
                neg++;
            if (pos > (b->size / 2) || neg > (b->size / 2))
                return false;
        }
    }
    //In Lines
    for (int i = 0; i < b->size; i++)
    {
        int pos = 0, neg = 0;
        for (int j = 0; j < b->size; j++)
        {
            if (b->data[j][i] == 1)
                pos++;
            else if (b->data[j][i] == -1)
                neg++;
            if (pos > (b->size / 2) || neg > (b->size / 2))
                return false;
        }
    }
    if (TripletDetect(b))
        return false;
    return true;
}
bool CmpRows(board *b)
{
    if (!b)
        return false;
    for (int i = 0; i < b->size - 1; i++)
        for (int j = i + 1; j < b->size; j++)
            for (int k = 0; k < b->size; k++)
            {
                if (b->data[i][k] != b->data[j][k])
                    break;
                if (k == b->size - 1)
                    return true;
            }
    return false;
}
bool CmpLines(board *b)
{
    if (!b)
        return false;
    for (int i = 0; i < b->size - 1; i++)
        for (int j = i + 1; j < b->size; j++)
            for (int k = 0; k < b->size; k++)
            {
                if (b->data[k][i] != b->data[k][j])
                    break;
                if (k == b->size - 1)
                    return true;
            }
    return false;
}
bool ValidSolution(board *b)
{
    if (!b)
        return false;
    //Verify the quantity of ones and zeros
    //In row
    for (int i = 0; i < b->size; i++)
    {
        int pos = 0, neg = 0;
        for (int j = 0; j < b->size; j++)
        {
            if (b->data[i][j] == 0) //Not fulfilled
                return false;
            else if (b->data[i][j] == 1)
                pos++;
            else if (b->data[i][j] == -1)
                neg++;
            else //Other illegal values exist
                return false;
            if (pos > b->size / 2 || neg > b->size / 2)
                return false;
        }
    }
    //In line
    for (int i = 0; i < b->size; i++)
    {
        int pos = 0, neg = 0;
        for (int j = 0; j < b->size; j++)
        {
            if (b->data[j][i] == 0) //Not fulfilled
                return false;
            else if (b->data[j][i] == 1)
                pos++;
            else if (b->data[j][i] == -1)
                neg++;
            else //Other illegal values exist
                return false;
            if (pos > b->size / 2 || neg > b->size / 2)
                return false;
        }
    }
    if (TripletDetect(b))
        return false;
    //Verify that there are no same rows and lines
    if (CmpRows(b))
        return false;
    if (CmpLines(b))
        return false;
    return true;
}
int CountAllBlank(board *b)
{
    if (!b)
        return -1;
    int count = 0;
    for (int i = 0; i < b->size; i++)
        for (int j = 0; j < b->size; j++)
            if (b->data[i][j] == 0)
                count++;
    return count;
}
Status FetchRandBlank(board *b, int *x, int *y)
{
    if (!b || !x || !y)
        return ERROR;
    int count = CountAllBlank(b);
    int chosen = rand() % count;
    for (int i = 0; i < b->size; i++)
        for (int j = 0; j < b->size; j++)
            if (b->data[i][j] == 0)
            {
                if (chosen > 0)
                    chosen--;
                else
                {
                    *x = i;
                    *y = j;
                    return OK;
                }
            }
    return ERROR;
}
board *NaiveSolve(board *b)
{
    pair *prev = InitPair(), *cur = NULL;
    board *solu = CopyBoard(b);
    Stack *S = InitStack();
    for (int i = 0; i < b->size; i++)
    {
        for (int j = 0; j < b->size; j++)
            if (b->data[i][j] == 0)
            {
                prev->x = i;
                prev->y = j;
                Push(S, prev);
                break;
            }
        if (!EmptyStack(S))
            break;
    }
    while (!EmptyStack(S))
    {
        prev = (pair *)Pop(S);
        //Reset
        solu->data[prev->x][prev->y] = 0;
        if (prev->info == -1)
            continue;
        for (int i = 0; i < b->size; i++)
        {
            for (int j = 0; j < b->size; j++)
            {
                if (solu->data[i][j] == 0)
                {
                    cur = InitPair();
                    cur->x = i;
                    cur->y = j;
                    if (prev->x == i && prev->y == j && prev->info != 0)
                        cur->info = -1;
                    else
                        cur->info = 1;
                    solu->data[i][j] = cur->info;
                    Push(S, cur);
                }
            }
            if (!ValidBoard(solu))
                break;
        }
        DestroyPair(prev);
        if (!CountAllBlank(solu))
            if (ValidSolution(solu))
            {
                for (int i = 0, count = S->top; i <= count; i++)
                    DestroyPair((pair *)Pop(S));
                DestroyStack(S);
                return solu;
            }
    }
    DestroyBoard(solu);
    DestroyStack(S);
    return NULL;
}
pair **FirstArrange(board *b, int line, int count)
{
    pair **ps = (pair **)malloc(sizeof(pair *) * count);
    if (!ps)
        exit(-1);
    bool *mark = (bool *)malloc(sizeof(bool) * b->size);
    for (int i = 0; i < b->size; i++)
        mark[i] = false;
    mark[0] = true;
    int d = 1;
    while (d != count)
        while (1)
        {
            int pos = rand() % (b->size);
            if (!mark[pos])
            {
                mark[pos] = true;
                d++;
                break;
            }
        }
    for (int i = 0; i < count; i++)
    {
        ps[i] = InitPair();
        int t = 0;
        while (!mark[t])
            t++;
        ps[i]->x = line;
        ps[i]->y = t;
        ps[i]->info = b->data[line][t];
        mark[t] = false;
    }
    free(mark);
    return ps;
}
bool FinalArrange(pair **ps, int size, int count)
{
    if (!ps || size <= 0 || count < 0)
        return false;
    if (count > size)
        return false;
    bool *mark = (bool *)malloc(sizeof(bool) * count);
    for (int i = 0; i < count; i++)
    {
        int back_pos = size - ps[i]->y;
        if (back_pos >= 0 && back_pos < count)
            mark[back_pos] = true;
        else
            mark[i] = false;
    }
    bool t = true;
    for (int i = 0; i < count; i++)
    {
        t = t && mark[i];
        if (!t)
            break;
    }
    free(mark);
    return t;
}
Status NextArrange(board *b, pair **ps, int line, int count)
{
    if (!ps || !b)
        return ERROR;
    if (line < 0 || count < 0 || count > b->size)
        return ERROR;
    if (FinalArrange(ps, b->size, count))
        return ERROR;
    //"int *temp" is designed to temporarily store the arrange sequence
    int *temp = (int *)malloc(sizeof(int) * count);
    //Copy current sequence reversively
    for (int i = 0; i < count; i++)
        temp[i] = ps[count - 1 - i]->y;
    //Detect final reach
    for (int i = 0; i < count; i++)
        if (temp[i] != b->size - 1 - i)
        {
            temp[i]++;
            //Move latter numbers to set a sub-initial status
            //when non-latest number got moved
            for (int j = i - 1; j >= 0; j--)
                temp[j] = temp[i] + i - j;
            break;
        }
    //Reassign, also reversively
    for (int i = count - 1; i >= 0; i--)
    {
        ps[count - 1 - i]->y = temp[i];
        ps[count - 1 - i]->info = b->data[line][temp[i]];
    }
    free(temp);
    return OK;
}
Status DestroyPairSet(pair **ps, int count)
{
    if (!ps || count <= 0)
        return ERROR;
    for (int i = 0; i < count; i++)
        DestroyPair(ps[i]);
    free(ps);
    return OK;
}
board *DigHoles(board *b, double ratio)
{
    if (!b || ratio > 1 || ratio < 0)
        return NULL;
    //Allocation
    int *count = (int *)malloc(sizeof(int) * b->size);
    if (!count)
        exit(-1);
    {
        //Limitations
        int upper_all = (int)(b->size * b->size * ratio);
        int upper_line = (int)(b->size * ratio);
        int remain = upper_all;
        for (int i = 0; i < b->size; i++)
        {
            if (!remain)
                count[i] = 0;
            else if (remain >= upper_line)
            {
                count[i] = upper_line;
                remain -= upper_line;
            }
            else
            {
                count[i] = remain;
                remain = 0;
            }
        }
    }
    Stack *S = InitStack();
    bool backtrack = false;
    do
    {
        pair **cur = NULL;
        int line = 0;
        if (backtrack)
        {
            cur = (pair **)Pop(S);
            //Reconstruct
            line = S->top + 1;
            for (int i = 0; i < count[line]; i++)
                b->data[line][cur[i]->y] = cur[i]->info;
            if (FinalArrange(cur, b->size, count[line]))
            {
                DestroyPairSet(cur, count[line]);
                continue;
            }
            NextArrange(b, cur, line, count[line]);
            backtrack = false;
        }
        else
        {
            line = S->top + 1;
            cur = FirstArrange(b, line, count[line]);
        }
        for (int i = 0; i < count[line]; i++)
        {
            b->data[line][cur[i]->y] *= -1;
            board *solu = NaiveSolve(b);
            if (solu)
            {
                DestroyBoard(solu);
                backtrack = true;
                break;
            }
            b->data[line][cur[i]->y] = 0;
        }
        Push(S, cur);
        if (!backtrack && line == b->size - 1)
        {
            for (int i = 0, num = S->top; i <= num; i++)
                DestroyPairSet((pair **)Pop(S), count[i]);
            DestroyStack(S);
            free(count);
            return b;
        }
    } while (!EmptyStack(S));
    DestroyStack(S);
    free(count);
    return NULL;
}
void RandGen(board *b, int num)
{
    if (!b)
        return;
    do
    {
        for (int i = 0; i < b->size; i++)
            for (int j = 0; j < b->size; j++)
                b->data[i][j] = 0;
        for (int i = 0; i < num; i++)
        {
            int x = 0, y = 0;
            FetchRandBlank(b, &x, &y);
            if (rand() % 2)
                b->data[x][y] = 1;
            else
                b->data[x][y] = -1;
        }
    } while (!ValidBoard(b));
    return;
}
board *GenBoard(int size, double dig_ratio)
{
    if (size % 2)
        return NULL;
    srand(time(NULL));
    //FILE *DSR = fopen("./test.txt", "r+");
    //board *b = ReadBoard(DSR);
    board *b = InitBoard(size);
    board *solu = NULL;
    do
    {
        RandGen(b, (int)(size * size * RAND_RATIO));
        solu = NaiveSolve(b);
    } while (!solu);
    //Terminal Reached
    b = DigHoles(solu, dig_ratio);
    return b;
}
int *BoardToVector(board *b)
{
    if (!b)
        return NULL;
    int *vec = (int *)malloc(sizeof(int) * (b->size * b->size + 1));
    if (!vec)
        exit(-1);
    vec[0] = 0;
    for (int i = 0; i < b->size; i++)
        for (int j = 0; j < b->size; j++)
            vec[i * b->size + j + 1] = b->data[i][j];
    return vec;
}
#endif
