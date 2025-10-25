#ifndef LIST_MANAGER_H_
#define LIST_MANAGER_H_

#include <stdlib.h>

typedef struct
{
    double value;
    int next;
    int previous;
} ListElement_t;

typedef struct
{
    size_t list_size;
    ListElement_t* elements;

    int head;
    int tail;
    int first_empty;

    FILE* log_file;
} List_t;

void ListInit       (List_t* list);
void ListDestroy    (List_t* list);
void ListAddAfter   (List_t* list, int index, double value);
void ListDelAfter   (List_t* list, int index);
void ListDel        (List_t* list, int index);

const size_t LIST_START_SIZE = 5;
const size_t LIST_MULTIPLIER_SIZE = 2;

#endif // LIST_MANAGER_H_