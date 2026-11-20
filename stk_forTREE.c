#include "STK_forTREE.h"

struct NODE *POIZON = NULL;

int StackVerify(STACK_PTR)
{
    if (stk == NULL)               return 1;
    if (stk->data == NULL)         return 2;
    if (stk->size > stk->capacity) return 3;

    if (stk->data[-2] != POIZON || stk->data[-1] != POIZON || stk->data[stk->capacity] != POIZON || stk->data[stk->capacity + 1] != POIZON)
        return 4;

    return 0;
}

void StackDump(STACK_PTR, const char *file, int line, const char *func)
{
    FILE *dump_file = fopen(FILE_TO_ERRORS, "w");
    if (dump_file == NULL) 
        dump_file = stdout;

    fprintf(dump_file, "================ STACK DUMP ================\n");
    fprintf(dump_file, "Ошибка в файле: %s | строка: %d | функция: %s\n", file, line, func);
    
    ON_DBG(fprintf(dump_file, "Имя переменной: \"%s\" | Создан: %s:%d\n", stk->name, stk->file, stk->line);)

    fprintf(dump_file, "Адрес структуры: %p\n", stk);
    
    if (stk != NULL)
    {
        fprintf(dump_file, "  size = %zu | capacity = %zu | data_ptr = %p\n",
                                stk->size, stk->capacity, (void *)stk->data);

        if (stk->data != NULL)
        {
            fprintf(dump_file, "--- Данные ---\n");
            for (size_t i = 0; i < stk->capacity + MEM_FOR_PIGEON; ++i)
                fprintf(dump_file, "[%zu] = %p\n", i, stk->data[i - 2]);
        }
    }
    fprintf(dump_file, "============================================\n");

    if (dump_file != stdout)
        fclose(dump_file);
}

int StackResize(STACK_PTR, size_t new_capacity)
{
    ASSERT(stk != NULL);
    ASSERT(new_capacity >= stk->size);

    STACK_ELEM *old_ptr = stk->data - 2;
    STACK_ELEM *new_ptr = (STACK_ELEM *)realloc(old_ptr, (new_capacity + MEM_FOR_PIGEON) * sizeof(STACK_ELEM));
    ASSERT(new_ptr != NULL);

    new_ptr[new_capacity + MEM_FOR_PIGEON - 1] = POIZON;
    new_ptr[new_capacity + MEM_FOR_PIGEON - 2] = POIZON;

    for (size_t i = stk->size + 2; i < new_capacity + 2; ++i)
        new_ptr[i] = 0;

    stk->data = new_ptr + 2;
    stk->capacity = new_capacity;

    return 0;
}

#ifdef STACK_DBG
int StackInit_(STACK_PTR, size_t capacity GET_ARGS_FOR_DBG)
#else
int StackInit(STACK_PTR, size_t capacity)
#endif
{
    ON_DBG(stk->name = name; stk->file = file; stk->line = line;)

    stk->capacity = capacity;
    stk->size = 0;

    STACK_ELEM *temp_ptr = (STACK_ELEM *)calloc(capacity + MEM_FOR_PIGEON, sizeof(STACK_ELEM));
    assert(temp_ptr != NULL);

    temp_ptr[0]                         = POIZON;
    temp_ptr[1]                         = POIZON;
    temp_ptr[capacity + MEM_FOR_PIGEON - 1] = POIZON;
    temp_ptr[capacity + MEM_FOR_PIGEON - 2] = POIZON;  //todo

    stk->data = temp_ptr + 2;

    STACK_ASSERT_OK(stk);
    return 0;
}

#ifdef STACK_DBG
int StackPush_(STACK_PTR, STACK_ELEM element GET_ARGS_FOR_DBG)
#else
int StackPush(STACK_PTR, STACK_ELEM element)
#endif
{
    STACK_ASSERT_OK(stk);

    int err = 0;
    if (stk->size + 1 >= stk->capacity)
        err = StackResize(stk, stk->capacity * 2);

    if (err != 0)
    {
        printf("не удалось увеличить память");
        exit(EXIT_FAILURE);
    }

    stk->data[stk->size] = element;
    stk->size += 1;
    
    STACK_ASSERT_OK(stk);
    return 0;
}

#ifdef STACK_DBG
STACK_ELEM StackPop_(STACK_PTR, const char *file, const int line)
#else
STACK_ELEM StackPop(STACK_PTR)
#endif
{
    STACK_ASSERT_OK(stk);

    stk->size -= 1;
    STACK_ELEM pop_res = stk->data[stk->size];
    stk->data[stk->size] = 0; 

    STACK_ASSERT_OK(stk);
    return pop_res;
}

void StackDestroy(STACK_PTR)
{
    STACK_ASSERT_OK(stk);

    free(stk->data - 2);
    stk->data = NULL;
    stk->size = 0;
    stk->capacity = 0;
}