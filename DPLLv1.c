#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#define EMPTY_SET 2
#define WITH_SIG_CLA 1
#define SPLIT_REQD 0
#define WITH_EMPTY_CLA -1
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
#define TRUE 1
#define OK 1
#define FALSE 0
#define ERROR 0
#define INFEASIBLE -1
#define OVERFLOW -1
#define INIT_SIZE 10
#define SIZE_INCREAMENT 2
typedef int Status;
typedef struct Stack
{
    void **data;
    int top;
    int size;
} Stack;
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
Status DestroyStack(Stack *S)
{
    if (!S)
        return ERROR;
    free(S->data);
    free(S);
    return OK;
}
Status ClearStack(Stack *S)
{
    if (!S)
        return ERROR;
    S->top = -1;
    return OK;
}
Status Push(Stack *S, void *e)
{
    if (!S)
        return ERROR;
    if (S->top + 1 > S->size)
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
Status IsEmpty(Stack *S)
{
    if (!S)
        return INFEASIBLE;
    if (S->top == 0)
        return TRUE;
    else
        return FALSE;
}
static int var_count, cla_count;
typedef struct Clause
{
    int var;
    struct Clause *next;
} Cla;
typedef struct Clause_Node
{
    Cla *data;
    int len;
    struct Clause_Node *next;
} ClaNode;
typedef struct SearchNode
{
    struct SearchNode *prev;
    ClaNode *head;
    struct SearchNode *lchild;
    struct SearchNode *rchild;
    int lmark;
    int rmark;
} SearchNode;
SearchNode *InitSearchNode(void);
ClaNode *CNF_Read(FILE *DSR);
ClaNode *Copy(ClaNode *head);
int FindSingleCla(SearchNode *root);
ClaNode *DeleteClaNode(ClaNode *head);
SearchNode *DeleteSearchNode(SearchNode *root);
void PSimplify(SearchNode *root, int tar);
void NSimplify(SearchNode *root, int tar);
SearchNode *Combine(int v, SearchNode *head);
int ChooseVarByOrder(SearchNode *root);
int ChooseVarByLogistics(SearchNode *root);
int ClaSetJudge(SearchNode *root);
bool DPLL(SearchNode *root, int *result);
int main()
{
    SearchNode *root = InitSearchNode();
    FILE *DSR = fopen("./eg/sat/L/eh-dp04s04.shuffled-1075.cnf", "r+");
    root->head = CNF_Read(DSR);
    int *result = (int *)malloc(sizeof(int) * var_count);
    clock_t start = clock();
    bool sat = DPLL(root, result);
    clock_t end = clock();
    double period = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    if (sat)
        printf("Satisfied.\n");
    else
        printf("Not satisfied.\n");
    printf("Time Cost: %.1f ms\n", period);
    return 0;
}
SearchNode *InitSearchNode(void)
{
    SearchNode *new = (SearchNode *)malloc(sizeof(SearchNode));
    if (!new)
        exit(-1);
    new->prev = NULL;
    new->head = NULL;
    new->lchild = NULL;
    new->rchild = NULL;
    new->lmark = 0;
    new->rmark = 0;
    return new;
}
ClaNode *CNF_Read(FILE *DSR)
{
    //Variables
    int d;
    char c = fgetc(DSR);
    char *line = (char *)malloc(sizeof(char) * 256);
    ClaNode *head = (ClaNode *)malloc(sizeof(ClaNode));
    if (!head)
        exit(-1);
    ClaNode *cur = head;
    //Skip notes in the *.cnf file
    while (c == 'c')
    {
        fgets(line, 256, DSR);
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
        cur->data = (Cla *)malloc(sizeof(Cla));
        if (!(cur->data))
            exit(-1);
        cur->data->next = NULL;
        cur->data->var = d;
        cur->len = 1;
        Cla *vrp = cur->data; //Variable Reading Pointer
        while (fscanf(DSR, "%d", &d), d != 0)
        {
            cur->len += 1;
            vrp->next = (Cla *)malloc(sizeof(Cla));
            vrp = vrp->next;
            vrp->var = d;
            vrp->next = NULL;
        }
        //Preparation for the next cycle
        if (fscanf(DSR, "%d", &d) != EOF)
        {
            cur->next = (ClaNode *)malloc(sizeof(ClaNode));
            cur = cur->next;
        }
        //Cycle ending
        else
        {
            cur->next = NULL;
            break;
        }
    }
    return head;
}
int ClaSetJudge(SearchNode *root)
{
    if (root->head == NULL)
        return EMPTY_SET;
    int mark = SPLIT_REQD;
    ClaNode *cur = root->head;
    while (cur)
    {
        if (cur->len == 0)
            return WITH_EMPTY_CLA;
        else if (cur->len == 1)
            mark = WITH_SIG_CLA;
        cur = cur->next;
    }
    return mark;
}
ClaNode *Copy(ClaNode *head)
{
    if (!head)
        return NULL;
    ClaNode *n_head = (ClaNode *)malloc(sizeof(ClaNode));
    if (!n_head)
        exit(-1);
    ClaNode *cur = head;
    ClaNode *n_cur = n_head;
    while (cur)
    {
        //Copying data chain
        Cla *vrp = cur->data;
        n_cur->data = (Cla *)malloc(sizeof(Cla));
        if (!(n_cur->data))
            exit(-1);
        Cla *n_vrp = n_cur->data;
        while (vrp)
        {
            n_vrp->var = vrp->var;
            if (vrp->next == NULL)
            {
                n_vrp->next = NULL;
                break;
            }
            else
            {
                n_vrp->next = (Cla *)malloc(sizeof(Cla));
                if (!(n_vrp->next))
                    exit(-1);
                n_vrp = n_vrp->next;
                vrp = vrp->next;
            }
        } //Copying complete
        //Copying external data
        n_cur->len = cur->len;
        //Prepare for the next cycle
        if (cur->next == NULL)
        {
            n_cur->next = NULL;
            break;
        }
        else
        {
            n_cur->next = (ClaNode *)malloc(sizeof(ClaNode));
            n_cur = n_cur->next;
            cur = cur->next;
        }
    }
    return n_head;
}
int FindSingleCla(SearchNode *root)
{
    //Strategy: return the first single clause found.
    ClaNode *cur = root->head;
    while (cur)
    {
        if (cur->len == 1)
            return cur->data->var;
        else
            cur = cur->next;
    }
    return 0;
}
ClaNode *DeleteClaNode(ClaNode *head)
{
    if (!head)
        return NULL;
    ClaNode *mark = head->next;
    if (head->len)
    {
        Cla *cur = head->data;
        Cla *next = NULL;
        while (cur)
        {
            next = cur->next;
            free(cur);
            cur = next;
        }
    }
    free(head);
    return mark;
}
SearchNode *DeleteSearchNode(SearchNode *root)
{
    if (!root)
        return NULL;
    SearchNode *prev = root->prev;
    ClaNode *cur = root->head;
    while (cur)
        cur = DeleteClaNode(cur);
    free(root);
    return prev;
}
void PSimplify(SearchNode *root, int tar)
{
    ClaNode *cur = root->head;
    ClaNode *prev = NULL;
    while (cur)
    {
        int status = 0;
        Cla *vrp = cur->data;
        while (vrp)
        {
            if (vrp->var == tar) //Match
            {
                status = 1; //Mark for deletion
                if (!prev)  //Reconstruct correct links
                    root->head = cur->next;
                else
                    prev->next = cur->next;
                DeleteClaNode(cur);
                break;
            }
            else
                vrp = vrp->next;
        }
        //Redirect pointers
        if (status)
        {
            if (!prev)
                cur = root->head;
            else
                cur = prev->next;
        }
        else
        {
            prev = cur;
            cur = cur->next;
        }
    }
    return;
}
void NSimplify(SearchNode *root, int tar)
{
    ClaNode *node = root->head;
    while (node)
    {
        Cla *cur = node->data;
        Cla *prev = NULL;
        while (cur)
        {
            if (cur->var == tar)
            {
                if (!prev)
                {
                    node->data = cur->next;
                    free(cur);
                    cur = node->data;
                }
                else
                {
                    prev->next = cur->next;
                    free(cur);
                    cur = prev->next;
                }
                node->len -= 1;
            }
            else
            {
                prev = cur;
                cur = cur->next;
            }
        }
        node = node->next;
    }
    return;
}
SearchNode *Combine(int v, SearchNode *root)
{
    SearchNode *n_root = (SearchNode *)malloc(sizeof(SearchNode));
    if (!n_root)
        exit(-1);
    ClaNode *n_head = (ClaNode *)malloc(sizeof(ClaNode));
    if (!n_head)
        exit(-1);
    n_head->data = (Cla *)malloc(sizeof(Cla));
    if (!(n_head->data))
        exit(-1);
    //Construct the new chain by adding a new head
    n_head->next = Copy(root->head);
    n_head->len = 1;
    n_head->data->var = v;
    n_head->data->next = NULL;
    //Construct the new search node by linking the new chain to it
    n_root->prev = root;
    n_root->head = n_head;
    n_root->lchild = NULL;
    n_root->rchild = NULL;
    n_root->lmark = 0;
    n_root->rmark = 0;
    return n_root;
}
int ChooseVarByOrder(SearchNode *root)
{
    if (!root || !(root->head) || !(root->head->data))
        return 0;
    return root->head->data->var;
}
int ChooseVarByLogistics(SearchNode *root)
{
    int *logist = (int *)malloc(sizeof(int) * (var_count + 1));
    for (int i = 0; i <= var_count; i++)
        logist[i] = 0;
    ClaNode *cur = root->head;
    while (cur)
    {
        Cla *vrp = cur->data;
        while (vrp)
        {
            if (vrp->var < 0)
                logist[(vrp->var) * -1]++;
            else
                logist[vrp->var]++;
            vrp = vrp->next;
        }
        cur = cur->next;
    }
    int max = 1;
    for (int i = 1; i <= var_count; i++)
        if (logist[i] > max)
            max = i;
    return max;
}
bool DPLL(SearchNode *root, int *result)
{
    int count = 0;
    int mark;
    int tar = 0;
    Stack *S = InitStack();
    SearchNode *cur = NULL;
    Push(S, root);
    while (S->top >= 0)
    {
        cur = Pop(S);
        if (!cur)
            return false;
        if (cur->lmark && cur->rmark)
        {
            Push(S, cur->prev);
            continue;
        }
        while (1) //Simplification when single clause(s) exist
        {
            mark = ClaSetJudge(cur);
            if (mark == WITH_SIG_CLA)
            {
                tar = FindSingleCla(cur);
                PSimplify(cur, tar);
                if (ClaSetJudge(cur) == EMPTY_SET)
                    return true;
                else
                    NSimplify(cur, tar * -1);
            }
            else if (mark == EMPTY_SET)
                return true;
            else
                break;
        }
        if (mark != WITH_EMPTY_CLA)
        {
            int v = ChooseVarByOrder(cur);
            if (cur->lchild == NULL)
            {
                cur->lchild = Combine(v, cur);
                cur->lmark = 1;
                Push(S, cur->lchild);
            }
            else if (cur->rchild == NULL)
            {
                cur->rchild = Combine(v * -1, cur);
                cur->rmark = 1;
                Push(S, cur->rchild);
            }
        }
        else
            Push(S, cur->prev);
    }
}