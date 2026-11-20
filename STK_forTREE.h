#ifndef STKfTREEH
#define STKfTREEH

#ifdef STACK_DBG
#define ON_DBG(...) __VA_ARGS__
#else
#define ON_DBG(...)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define FILE_TO_ERRORS "stack_dump.txt"
#define STACK_PTR struct stack_t *stk
#define MEM_FOR_PIGEON 4
#define GET_ARGS_FOR_DBG , const char *name, const char *file, const int line

struct NODE;

typedef struct NODE* STACK_ELEM;

#define STACK_ASSERT_OK(stk)                                \
    if (StackVerify(stk) != 0)                              \
    {                                                       \
        printf("%d", StackVerify(stk));                     \
        StackDump(stk, __FILE__, __LINE__, __FUNCTION__);   \
        exit(EXIT_FAILURE);                                 \
    }   
    
 //naming
#define ASSERT(arg)   \
    if ((arg) == 0)   \
        return 1;     

//struct NODE *POIZON = NULL;

struct stack_t //need canary for struct
{
    size_t     size;
    size_t     capacity;
    STACK_ELEM *data;
    ON_DBG (const char *name; const char *file; int line;)
};

#ifdef STACK_DBG

    #define StackInit(stk, capacity) StackInit_((stk), (capacity), #stk, __FILE__, __LINE__)
    int StackInit_(STACK_PTR, size_t capacity GET_ARGS_FOR_DBG);

    #define StackPush(stk, elem) StackPush_((stk), (elem), #elem, __FILE__, __LINE__)
    int StackPush_(STACK_PTR, STACK_ELEM element GET_ARGS_FOR_DBG);

    #define StackPop(stk) StackPop_((stk), __FILE__, __LINE__)
    STACK_ELEM StackPop_(STACK_PTR const char *file, const int line);

#else

    int        StackInit   (STACK_PTR, size_t capacity);
    int        StackPush   (STACK_PTR, STACK_ELEM element);
    STACK_ELEM StackPop    (STACK_PTR);

#endif

void StackDestroy(STACK_PTR);
int  StackVerify (STACK_PTR);
void StackDump   (STACK_PTR, const char *file, const int line, const char *func);

#endif