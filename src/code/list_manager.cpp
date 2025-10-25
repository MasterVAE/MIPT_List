#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "list_manager.h"
#include "list_logger.h"

static void ReallocList(List_t* list);

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

    FILE* file = fopen("logger.html", "w+");
    //FIXME file check
    CreateBaseHTML(file);

    list->log_file = file;
}

void ListDestroy(List_t* list)
{
    assert(list);
    free(list->elements);
    fclose(list->log_file);
    memset(list, 0, sizeof(List_t));
}

//FIXME non void
void ListAddAfter(List_t* list, int index, double value)
{
    assert(list);

    //FIXME ERROR
    if(index < 0) return;
    if(index > (int)list->list_size) return;
    if(list->elements[index].previous < 0) return;

    if(list->first_empty >= (int)list->list_size)
    {
        ReallocList(list);
    }

    int added_index = list->first_empty;
    list->first_empty = abs(list->elements[added_index].next);

    list->elements[added_index].value = value;
    list->elements[added_index].previous = index;
    list->elements[added_index].next = list->elements[index].next;

    if(list->elements[added_index].next != 0) 
    {
        list->elements[list->elements[added_index].next].previous = added_index;
    }
    if(index != 0)
    {
        list->elements[index].next = added_index;
    }
    
    list->tail = added_index;

    if(index == 0)
    {
        list->head = 1;
    }

    ListDump(list);
}

//FIXME non void
void ListDel(List_t* list, int index)
{
    assert(list);

    //FIXME ERROR
    if(index < 0) return;
    if(index > (int)list->list_size) return;
    if(list->elements[index].previous < 0) return;

    ListElement_t* deleting_elem = &list->elements[index];

    if(deleting_elem->next > 0)
    {
        list->elements[deleting_elem->next].previous = deleting_elem->previous;
    }
    if(deleting_elem->previous > 0)
    {
        list->elements[deleting_elem->previous].next = deleting_elem->next;
    }    
    
    list->elements[index].next = -list->first_empty;
    list->elements[index].previous = -1;
    list->first_empty = index;

    ListDump(list);
}

static void ReallocList(List_t* list)
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
    for(int i = list->first_empty; i < (int)list->list_size; i++)
    {
        list->elements[i].value = 0;
        list->elements[i].next = -((int)i + 1);
        list->elements[i].previous = -1;
    }
}