
#include <stdio.h>                                                                                 
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "list_manager.h"
#include "list_logger.h"

static ListErr ReallocList(List_t* list);

const char* LOGGER_FILENAME = "files/logger.html";

// ИНИЦИАЛИЗАЦИЯ СПИСКА
ListErr ListInit(List_t* list)
{
    if(!list) return LIST_NULL;

    list->list_capacity = LIST_START_CAPACITY;
    list->elements = (ListElement_t*)calloc(LIST_START_CAPACITY, sizeof(ListElement_t));
    if(!list->elements) return LIST_MEMORY_ERROR;

    list->head = 0;
    list->tail = 0;

    list->first_empty = 1;

    

    for(size_t element = 0; element < list->list_capacity; element++)
    {
        if(element == 0)
        {
            list->elements[element].value = SHIELD_VALUE;
            list->elements[element].next = 0;
            list->elements[element].previous = 0;
        }
        else
        {
            list->elements[element].value = POISON;
            list->elements[element].next = -((int)element + 1);
            list->elements[element].previous = -1;
        }
    }

    FILE* file = fopen(LOGGER_FILENAME, "w+");
    if(!file) return LIST_FILE_ERROR;
    StartBaseHTML(file);

    list->log_file = file;

    VERIFY(list);

    return LIST_CORRECT;
}

// УНИЧТОЖЕНИЕ СПИСКА
void ListDestroy(List_t* list)
{
    if(!list)  return;

    EndBaseHTML(list->log_file);

    free(list->elements);
    if(list->log_file)
    {
        fclose(list->log_file);
    }
    memset(list, 0, sizeof(List_t));
}

// ВСТАВКА ПОСЛЕ
ListErr ListAddAfter(List_t* list, int index, double value)
{
    if(!list)       return LIST_NULL;
    if(index < 0 
    || index > (int)list->list_capacity 
    || list->elements[index].previous < 0)                          
                    return LIST_INCORRECT_INDEX;

    VERIFY(list);

    if(list->first_empty >= (int)list->list_capacity)
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
    
    if(list->tail == index)
    {
        list->tail = added_index;
    }

    if(index == 0)
    {
        list->head = 1;
    }

    ListDump(list);

    VERIFY(list);

    return LIST_CORRECT;
}

// УДАЛЕНИЕ ЭЛЕМЕНТА
ListErr ListDel(List_t* list, int index)
{
    if(!list)                               return LIST_NULL;
    if(index < 0)                           return LIST_INCORRECT_INDEX;
    if(index > (int)list->list_capacity)    return LIST_INCORRECT_INDEX;
    if(list->elements[index].previous < 0)  return LIST_INCORRECT_INDEX;

    VERIFY(list);

    ListElement_t* deleting_elem = &list->elements[index];

    if(list->tail == index)
    {
        list->tail = deleting_elem->previous;
    }

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

    VERIFY(list);

    return LIST_CORRECT;
}

int ListNext(List_t* list, int index)
{
    if(!list                               
    || index <= 0                      
    || index > (int)list->list_capacity
    || list->elements[index].previous < 0)  return 0;

    return list->elements[index].next;
}

int ListPrev(List_t* list, int index)
{
    if(!list                               
    || index <= 0                      
    || index > (int)list->list_capacity
    || list->elements[index].previous < 0)  return 0;

    return list->elements[index].previous;
}

// УДЛИНЕНИЕ СПИСКА
static ListErr ReallocList(List_t* list)
{
    VERIFY(list);

    list->list_capacity *= LIST_MULTIPLIER_CAPACITY;
    list->elements = (ListElement_t*)realloc(list->elements, 
                                                    list->list_capacity * sizeof(ListElement_t));
    if(!list->elements)
    {
        free(list->elements);
        return LIST_MEMORY_ERROR;
    }

    for(int element = list->first_empty; element < (int)list->list_capacity; element++)
    {
        list->elements[element].value = POISON;
        list->elements[element].next = -((int)element + 1);
        list->elements[element].previous = -1;
    }

    VERIFY(list);

    return LIST_CORRECT;
}

// ВЫВОД ОШИБКИ
void PrintError(ListErr errcode)
{
    switch (errcode)
    {
        case LIST_NULL:
            fprintf(stderr, "ERROR: list is null\n");
            break;
        case LIST_FILE_ERROR:
            fprintf(stderr, "ERROR: list file error\n");
            break;
        case LIST_INCORRECT_INDEX:
            fprintf(stderr, "ERROR: incorrect index\n");
            break;
        case LIST_MEMORY_ERROR:
            fprintf(stderr, "ERROR: memory error\n");
            break;
        case LIST_INVALID_HEAD:
            fprintf(stderr, "ERROR: invalid head\n");
            break;
        case LIST_INVALID_TAIL:
            fprintf(stderr, "ERROR: invalid tail\n");
            break;
        case LIST_INVALID_EMPTY:
            fprintf(stderr, "ERROR: invalid empty\n");
            break;
        case LIST_SHEILD_DAMAGED:
            fprintf(stderr, "ERROR: shield damaged\n");
            break;
        case LIST_CORRECT:
            fprintf(stderr, "CORRECT\n");
            break;
        default:
            break;
    }   
}

// ПРОВЕРЯТОР
ListErr ListVerify(List_t* list)
{
    if(!list)               return LIST_NULL;

    if(!list->elements)     return LIST_MEMORY_ERROR;

    if(list->elements[0].value != SHIELD_VALUE)
                            return LIST_SHEILD_DAMAGED;

    if(list->head < 0 
    || list->elements[list->head].previous < 0
    || list->head >= (int)list->list_capacity) 
                            return LIST_INVALID_HEAD;

    if(list->tail < 0 
    || list->elements[list->tail].previous < 0
    || list->tail >= (int)list->list_capacity) 
                            return LIST_INVALID_TAIL;

    if(list->first_empty < 0)
                            return LIST_INVALID_TAIL;

    if(list->first_empty < (int)list->list_capacity
    && list->elements[list->first_empty].previous >= 0)
                            return LIST_INVALID_TAIL;

    return LIST_CORRECT;
}