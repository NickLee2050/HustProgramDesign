#include <stdlib.h>
#include <stdbool.h>
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
#ifndef __BOARD__CONVERT__
#define __BOARD__CONVERT__
ClaNode *TripletAvoid(int size, ClaNode *head)
{
    ClaNode *prev = head;
    //In row
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size - 2; j++)
        {
            ClaNode *cur = InitCla();
            cur->next = InitCla();
            cla_count += 2;
            for (int k = j; k <= j + 2; k++)
                AddToCla(cur, i * size + k + 1);
            for (int k = j; k <= j + 2; k++)
                AddToCla(cur->next, (i * size + k + 1) * -1);
            cur->next->next = prev;
            prev = cur;
        }
    //In line
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size - 2; j++)
        {
            ClaNode *cur = InitCla();
            cur->next = InitCla();
            cla_count += 2;
            for (int k = j; k <= j + 2; k++)
                AddToCla(cur, k * size + i + 1);
            for (int k = j; k <= j + 2; k++)
                AddToCla(cur->next, (k * size + i + 1) * -1);
            cur->next->next = prev;
            prev = cur;
        }
    return prev;
}
ClaNode *HalfVar(int size, ClaNode *head)
{
    if (size % 2)
        return NULL;
    int count = size / 2 + 1;
    ClaNode *prev = head;
    int *arrange = (int *)malloc(sizeof(int) * count);
    if (!arrange)
        exit(-1);
    for (int i = 0; i < size; i++)
    {
        //Initialization
        for (int j = 0; j < count; j++)
            arrange[j] = j;
        //Detect final reach
        bool final_reach;
        do
        {
            //Create new constraints
            //In row
            {
                ClaNode *cur = InitCla();
                cur->next = InitCla();
                cla_count += 2;
                for (int k = 0; k < count; k++)
                    AddToCla(cur, i * size + arrange[k] + 1);
                for (int k = 0; k < count; k++)
                    AddToCla(cur->next, (i * size + arrange[k] + 1) * -1);
                cur->next->next = prev;
                prev = cur;
            }
            //In line
            {
                ClaNode *cur = InitCla();
                cur->next = InitCla();
                cla_count += 2;
                for (int k = 0; k < count; k++)
                    AddToCla(cur, arrange[k] * size + i + 1);
                for (int k = 0; k < count; k++)
                    AddToCla(cur->next, (arrange[k] * size + i + 1) * -1);
                cur->next->next = prev;
                prev = cur;
            }

            //Assume that in this cycle, we would reach final arrange
            final_reach = true;
            for (int j = count - 1; j >= 0; j--)
            {
                if (arrange[j] != size + j - count)
                {
                    arrange[j]++;
                    //Move latter numbers to set a sub-initial status
                    //when non-latest number got moved
                    for (int k = j + 1; k < count; k++)
                        arrange[k] = arrange[j] + k - j;
                    //Any adjustment means we haven't reached final arrange
                    final_reach = false;
                    break;
                }
            }
        } while (!final_reach); //Terminate when reached final arrange
    }
    return prev;
}
ClaNode *TseytinAddInRow(int i, int j, int size, ClaNode *head)
{
    ClaNode *prev = head;
    //Positive Group
    for (int k = 0; k < size; k++)
    {
        var_count++;
        for (int l = 0; l < 3; l++)
        {
            ClaNode *cur = InitCla();
            cla_count++;
            cur->next = prev;
            prev = cur;
            switch (l)
            {
            case 0:
            {
                AddToCla(cur, i * size + k + 1);
                AddToCla(cur, var_count * -1);
                break;
            }
            case 1:
            {
                AddToCla(cur, j * size + k + 1);
                AddToCla(cur, var_count * -1);
                break;
            }
            case 2:
            {
                AddToCla(cur, (i * size + k + 1) * -1);
                AddToCla(cur, (j * size + k + 1) * -1);
                AddToCla(cur, var_count);
                break;
            }
            }
        }
    }
    //Negative Group
    for (int k = 0; k < size; k++)
    {
        var_count++;
        for (int l = 0; l < 3; l++)
        {
            ClaNode *cur = InitCla();
            cla_count++;
            cur->next = prev;
            prev = cur;
            switch (l)
            {
            case 0:
            {
                AddToCla(cur, (i * size + k + 1) * -1);
                AddToCla(cur, var_count * -1);
                break;
            }
            case 1:
            {
                AddToCla(cur, (j * size + k + 1) * -1);
                AddToCla(cur, var_count * -1);
                break;
            }
            case 2:
            {
                AddToCla(cur, i * size + k + 1);
                AddToCla(cur, j * size + k + 1);
                AddToCla(cur, var_count);
                break;
            }
            }
        }
    }
    //OR for both groups above
    for (int k = 0; k < size; k++)
    {
        var_count++;
        for (int l = 0; l < 3; l++)
        {
            ClaNode *cur = InitCla();
            cla_count++;
            cur->next = prev;
            prev = cur;
            switch (l)
            {
            case 0:
            {
                AddToCla(cur, (var_count - size) * -1);
                AddToCla(cur, var_count);
                break;
            }
            case 1:
            {
                AddToCla(cur, (var_count - 2 * size) * -1);
                AddToCla(cur, var_count);
                break;
            }
            case 2:
            {
                AddToCla(cur, var_count - size);
                AddToCla(cur, var_count - 2 * size);
                AddToCla(cur, var_count * -1);
                break;
            }
            }
        }
    }
    //Final AND
    var_count++;
    ClaNode *final = InitCla();
    cla_count++;
    final->next = prev;
    prev = final;
    AddToCla(final, var_count * -1);
    for (int k = 0; k < size; k++)
    {
        ClaNode *cur = InitCla();
        cla_count++;
        cur->next = prev;
        prev = cur;
        AddToCla(final, (var_count - k - 1) * -1);
        AddToCla(cur, var_count - k - 1);
        AddToCla(cur, var_count);
    }
    ClaNode *cur = InitCla();
    cla_count++;
    cur->next = prev;
    prev = cur;
    AddToCla(cur, var_count);
    return prev;
}
ClaNode *TseytinAddInLine(int i, int j, int size, ClaNode *head)
{
    ClaNode *prev = head;
    //Positive Group
    for (int k = 0; k < size; k++)
    {
        var_count++;
        for (int l = 0; l < 3; l++)
        {
            ClaNode *cur = InitCla();
            cla_count++;
            cur->next = prev;
            prev = cur;
            switch (l)
            {
            case 0:
            {
                AddToCla(cur, k * size + i + 1);
                AddToCla(cur, var_count * -1);
                break;
            }
            case 1:
            {
                AddToCla(cur, k * size + j + 1);
                AddToCla(cur, var_count * -1);
                break;
            }
            case 2:
            {
                AddToCla(cur, (k * size + i + 1) * -1);
                AddToCla(cur, (k * size + j + 1) * -1);
                AddToCla(cur, var_count);
                break;
            }
            }
        }
    }
    //Negative Group
    for (int k = 0; k < size; k++)
    {
        var_count++;
        for (int l = 0; l < 3; l++)
        {
            ClaNode *cur = InitCla();
            cla_count++;
            cur->next = prev;
            prev = cur;
            switch (l)
            {
            case 0:
            {
                AddToCla(cur, (k * size + i + 1) * -1);
                AddToCla(cur, var_count * -1);
                break;
            }
            case 1:
            {
                AddToCla(cur, (k * size + j + 1) * -1);
                AddToCla(cur, var_count * -1);
                break;
            }
            case 2:
            {
                AddToCla(cur, k * size + i + 1);
                AddToCla(cur, k * size + j + 1);
                AddToCla(cur, var_count);
                break;
            }
            }
        }
    }
    //OR for both groups above
    for (int k = 0; k < size; k++)
    {
        var_count++;
        for (int l = 0; l < 3; l++)
        {
            ClaNode *cur = InitCla();
            cla_count++;
            cur->next = prev;
            prev = cur;
            switch (l)
            {
            case 0:
            {
                AddToCla(cur, (var_count - size) * -1);
                AddToCla(cur, var_count);
                break;
            }
            case 1:
            {
                AddToCla(cur, (var_count - 2 * size) * -1);
                AddToCla(cur, var_count);
                break;
            }
            case 2:
            {
                AddToCla(cur, var_count - size);
                AddToCla(cur, var_count - 2 * size);
                AddToCla(cur, var_count * -1);
                break;
            }
            }
        }
    }
    //Final AND
    var_count++;
    ClaNode *final = InitCla();
    cla_count++;
    final->next = prev;
    prev = final;
    AddToCla(final, var_count * -1);
    for (int k = 0; k < size; k++)
    {
        ClaNode *cur = InitCla();
        cla_count++;
        cur->next = prev;
        prev = cur;
        AddToCla(final, (var_count - k - 1) * -1);
        AddToCla(cur, var_count - k - 1);
        AddToCla(cur, var_count);
    }
    //Ensure the condition satisfies
    ClaNode *cur = InitCla();
    cla_count++;
    cur->next = prev;
    prev = cur;
    AddToCla(cur, var_count);
    return prev;
}
ClaNode *SameArrangeAvoid(int size, ClaNode *head)
{
    //Using Tseytin Transformation

    ClaNode *prev = head;
    ClaNode *cur = NULL;
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
        {
            cur = TseytinAddInRow(i, j, size, prev);
            prev = cur;
        }
    for (int i = 0; i < size - 1; i++)
        for (int j = i + 1; j < size; j++)
        {
            cur = TseytinAddInLine(j, i, size, prev);
            prev = cur;
        }
    return prev;
}
int FindSingleCla(ClaNode *head)
{
    ClaNode *cur = head;
    while (cur)
    {
        if (cur->len == 1)
            return cur->data[0];
        cur = cur->next;
    }
    return 0;
}
ClaNode *ConvertBoard(board *b)
{
    if (!b)
        return NULL;
    var_count = b->size * b->size;
    cla_count = 0;
    ClaNode *head = SameArrangeAvoid(b->size, NULL);
    head = HalfVar(b->size, head);
    head = TripletAvoid(b->size, head);
    ClaNode *prev = head;
    ClaNode *cur = NULL;
    for (int i = 0; i < b->size; i++)
        for (int j = 0; j < b->size; j++)
            if (b->data[i][j] != 0)
            {
                cur = InitCla();
                cur->next = prev;
                if (b->data[i][j] > 0)
                    AddToCla(cur, i * b->size + j + 1);
                else
                    AddToCla(cur, (i * b->size + j + 1) * -1);
                prev = cur;
            }
    return prev;
}
#endif