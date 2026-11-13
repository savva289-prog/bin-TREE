#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int LIST_ELEM;

const int POIZON = -13;

typedef struct list_elem
{
    LIST_ELEM elem;
    size_t prev;
} list_elem;

typedef struct list_t
{
    size_t free;
    size_t head;
    size_t tail;
    size_t capacity;
    size_t *next;
    struct list_elem *data;
} list_t;

void list_init(list_t *list, size_t start_capacity);
void LIST_add(list_t *list, LIST_ELEM elem, size_t target_number);

int main()
{
    list_t lst = {0};
    list_init(&lst, 16);

    // Добиваемся порядка: 10, 40, 20, 30
    // target_number здесь — это индекс узла, ПОСЛЕ которого мы вставляем элемент.
    // 0 — это нулевой барьерный узел (начало/конец кольца).
    LIST_add(&lst, 10, 1); // Вставляем после барьера (стает первым)
    LIST_add(&lst, 20, 2); // Вставляем после узла 1 (после 10)
    LIST_add(&lst, 30, 3); // Вставляем после узла 2 (после 20)
    LIST_add(&lst, 40, 2); // Вставляем после узла 1 (между 10 и 20)

    // Обход списка по кольцу через нулевой барьер
    printf("Обход списка (10 -> 40 -> 20 -> 30):\n");
    size_t curr = 1;
    for(int i = 0; i < 5; ++i)
    {
        printf("(элемент: %d, индекс: %zu, след: %zu, пред: %zu)\n", 
               lst.data[curr].elem, curr, lst.next[curr], lst.data[curr].prev);
        curr = lst.next[curr];
    }

    // Освобождение памяти
    free(lst.data);
    free(lst.next);
    return 0;
}

void list_init(list_t *list, size_t start_capacity)
{
    assert(list != NULL);
    assert(start_capacity != 0);

    list->data = (list_elem *)calloc(start_capacity, sizeof(list_elem));
    list->next = (size_t *)calloc(start_capacity, sizeof(size_t));
    assert(list->data != NULL);
    assert(list->next != NULL);

    for (size_t i = 1; i < start_capacity - 1; ++i)
    {
        list->next[i] = i + 1;
        list->data[i].elem = POIZON;
    }

    list->next[start_capacity - 1] = 0; 
    list->data[start_capacity - 1].elem = POIZON;

    list->data[0].elem = POIZON;
    list->data[0].prev = 0;
    list->next[0] = 0;

    list->head = 0;
    list->tail = 1;

    list->free = 1;
    list->capacity = start_capacity;
}

void LIST_add(list_t *list, LIST_ELEM elem, size_t target_number)
{
    assert(list != NULL);
    //assert(list->free != 0); // Проверка на переполнение

    size_t new_node = list->free;
    list->free = list->next[new_node];

    list->data[new_node].elem = elem;

    size_t next_node = list->next[target_number];

    list->next[new_node] = next_node;
    list->data[new_node].prev = list->data[target_number].prev;

    list->next[list->data[target_number].prev] = new_node;
    list->data[next_node].prev = new_node;

    list->head = (target_number > list->head) ? target_number : list->head;
    list->tail = (target_number < list->tail) ? target_number : list->tail;

    list->next[list->head] = list->tail;
    list->data[list->tail].prev = list->head;
}
