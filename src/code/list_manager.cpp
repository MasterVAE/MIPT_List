#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "list_manager.h"

//FIXME non void
void ListInit(List_t* list)
{
    assert(list);

    list->list_size = LIST_START_SIZE;
    list->elements = (ListElement_t*)calloc(LIST_START_SIZE, sizeof(ListElement_t));

    list->head = 0;
    list->tail = 0;

    list->first_empty = 1;

    for(size_t element = 0; element < list->list_size; element++)
    {
        if(element == 0)
        {
            list->elements[element].next = 0;
            list->elements[element].previous = 0;
        }
        else
        {
            list->elements[element].next = -((int)element + 1);
            list->elements[element].previous = -1;
        }
    }
}

void ListDestroy(List_t* list)
{
    assert(list);
    free(list->elements);
    memset(list, 0, sizeof(List_t));
}

void ListDump(List_t* list)
{
    assert(list);

    printf("========== LIST DUMP START ============\n\n");

    printf("HEAD:      %4d\n",   list->head);
    printf("TAIL:      %4d\n",   list->tail);
    printf("LIST SIZE: %4lu\n",  list->list_size);
    printf("NEXT EMPTY:%4d\n\n", list->first_empty);

    for(size_t element = 0; element < list->list_size; element++)
    {
        printf("[%2lu] VALUE: %.5f   NEXT: %3d   PREVIOUS:  %3d\n", 
                                                            element,
                                                            list->elements[element].value, 
                                                            list->elements[element].next,
                                                            list->elements[element].previous);
    }

    printf("\n========== LIST DUMP END   ============\n\n");

    // template = "HEAD %4d %s %d";

    // sprintf(template)
}

//FIXME non void
void ListAddAfter(List_t* list, int index, double value)
{
    assert(list);

    if(list->first_empty >= list->list_size)
    {
        list->list_size *= LIST_MULTIPLIER_SIZE;
        list->elements = (ListElement_t*)realloc(list->elements, 
                                                        list->list_size * sizeof(ListElement_t));
        if(!list->elements)
        {
            free(list->elements);
            return;
        }
        //FIXME i
        for(int i = list->first_empty; i < list->list_size; i++)
        {
            list->elements[i].value = 0;
            list->elements[i].next = -((int)i + 1);
            list->elements[i].previous = -1;
        }
    }

    int empty = list->first_empty;
    list->elements[index].next = empty;
    list->elements[empty].value = value;
    list->elements[empty].previous = index;

    list->head = empty;
    list->first_empty = abs(list->elements[empty].next);

    ListDump(list);
}