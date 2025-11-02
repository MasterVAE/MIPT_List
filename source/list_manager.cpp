#include <stdio.h>                                                                                 
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "../include/list_manager.h"
#include "../include/list_logger.h"

static ListErr ReallocList(List_t* list, ReallocType type);

static const char* const LOGGER_FILENAME = "files/logger.html";
const int POISON_PREVIOUS = -1;

// ИНИЦИАЛИЗАЦИЯ СПИСКА
ListErr ListInit(List_t* list)
{
    if(!list) return LIST_NULL;

    list->list_capacity = LIST_START_CAPACITY;
    list->elements = (ListElement_t*)calloc(LIST_START_CAPACITY, sizeof(ListElement_t));
    if(!list->elements) return LIST_MEMORY_ERROR;

    list->first_empty = 1;

    list->elements[0].value = SHIELD_VALUE;
    list->elements[0].next = 0;
    list->elements[0].previous = 0;

    for(size_t element = 1; element < list->list_capacity; element++)
    {
        list->elements[element].value = POISON;
        list->elements[element].next = -((int)element + 1);
        list->elements[element].previous = POISON_PREVIOUS;
    }

    FILE* file = fopen(LOGGER_FILENAME, "w+");
    if(!file) return LIST_FILE_ERROR;
    StartBaseHTML(file);

    list->log_file = file;
    list->list_size = 1;

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
    free(list);
}

// ВСТАВКА ПОСЛЕ
ListErr ListAddAfter(List_t* list, int index, list_type value)
{
    if(!list)       return LIST_NULL;
    if(index < 0 
    || index > (int)list->list_capacity 
    || list->elements[index].previous < 0)                          
                    return LIST_INCORRECT_INDEX;

    VERIFY(list);

    if(list->list_size >= list->list_capacity)
    {
        ListErr err = ReallocList(list, REALLOC_UP);
        CHECK(err);
    }

    int added_index = list->first_empty;
    list->first_empty = abs(list->elements[added_index].next);

    list->elements[added_index].value = value;
    list->elements[added_index].previous = index;
    list->elements[added_index].next = list->elements[index].next;

    list->elements[list->elements[added_index].next].previous = added_index;

    list->elements[index].next = added_index;

    list->list_size++;

    VERIFY(list);

    return LIST_CORRECT;
}

// ВСТАВКА ПЕРЕД
ListErr ListAddBefore(List_t* list, int index, list_type value)
{
    if(!list)       return LIST_NULL;
    if(index < 0 
    || index > (int)list->list_capacity 
    || list->elements[index].previous < 0)                          
                    return LIST_INCORRECT_INDEX;

    return ListAddAfter(list, list->elements[index].previous, value);
}

// УДАЛЕНИЕ ЭЛЕМЕНТА
ListErr ListDel(List_t* list, int index)
{
    if(!list)                               return LIST_NULL;
    if(index <= 0  
    || index > (int)list->list_capacity
    || list->elements[index].previous < 0)  return LIST_INCORRECT_INDEX;

    VERIFY(list);

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
    list->elements[index].previous = POISON_PREVIOUS;
    list->elements[index].value = POISON;
    list->first_empty = index;

    list->list_size--;

    if(list->list_size <= list->list_capacity / LIST_MULTIPLIER_CAPACITY)
    {
        ListErr err = ReallocList(list, REALLOC_DOWN);
        CHECK(err);
    }

    VERIFY(list);

    return LIST_CORRECT;
}

size_t ListNext(List_t* list, int index)
{
    if(!list                               
    || index < 0                      
    || index > (int)list->list_capacity
    || list->elements[index].previous < 0)  return 0;

    return (size_t)list->elements[index].next;
}

size_t ListPrev(List_t* list, int index)
{
    if(!list                               
    || index < 0                      
    || index > (int)list->list_capacity
    || list->elements[index].previous < 0)  return 0;

    return (size_t)list->elements[index].previous;
}

// ИЗМЕНЕНИЕ ДЛИНЫ СПИСКА
static ListErr ReallocList(List_t* list, ReallocType type)
{
    VERIFY(list);

    if(type == REALLOC_UP)  list->list_capacity *= LIST_MULTIPLIER_CAPACITY;
    else  list->list_capacity /= LIST_MULTIPLIER_CAPACITY;

    ListElement_t* list_elem = (ListElement_t*)calloc(list->list_capacity, sizeof(ListElement_t));
    if(!list_elem)
    {
        free(list->elements);
        return LIST_MEMORY_ERROR;
    }

    int index = 0;
    int absolute_index = 0;
    do
    {
        list_elem[absolute_index].value = list->elements[index].value;
        list_elem[absolute_index].next = list->elements[index].next == 0 ? 0 : absolute_index + 1;
        list_elem[absolute_index].previous = index == 0 ? 
                                                (int)list->list_size - 1 : absolute_index - 1;
        absolute_index++;
        index = ListNext(list, index);
    } while (index != 0);
    
    list->first_empty = absolute_index;
    for(int element = list->first_empty; element < (int)list->list_capacity; element++)
    {
        list_elem[element].value = POISON;
        list_elem[element].next = -((int)element + 1);
        list_elem[element].previous = POISON_PREVIOUS;
    }

    free(list->elements);
    list->elements = list_elem;

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
        case LIST_INVALID_SIZE:
            fprintf(stderr, "ERROR: invalid size\n");
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

    if(list->list_size > list->list_capacity)     return LIST_MEMORY_ERROR;

    if(!ValueEquality(list->elements[0].value, SHIELD_VALUE))
                            return LIST_SHEILD_DAMAGED;

    if(list->elements[0].next < 0 
    || list->elements[list->elements[0].next].previous < 0
    || list->elements[0].next >= (int)list->list_capacity) 
                            return LIST_INVALID_HEAD;

    if(list->elements[0].previous < 0 
    || list->elements[list->elements[0].previous].previous < 0
    || list->elements[0].previous>= (int)list->list_capacity) 
                            return LIST_INVALID_TAIL;

    if(list->first_empty < 0)
                            return LIST_INVALID_TAIL;

    if(list->first_empty < (int)list->list_capacity
    && list->elements[list->first_empty].previous >= 0)
                            return LIST_INVALID_TAIL;

    size_t real_size = 1;
    int elem_index = list->elements[0].next;
    while(elem_index > 0 && real_size < list->list_size + 1)
    {
        elem_index = list->elements[elem_index].next;
        real_size++;
    }
    if(real_size != list->list_size) 
                            return LIST_INVALID_SIZE;

    real_size = 1;
    elem_index = list->elements[0].previous;
    while(elem_index > 0 && real_size < list->list_size + 1)
    {
        elem_index = list->elements[elem_index].previous;
        real_size++;
    }
    if(real_size != list->list_size) 
                            return LIST_INVALID_SIZE;

    return LIST_CORRECT;
}

size_t ListHead(List_t* list)
{
    if(!list) return 0;

    return list->elements[0].next;
}

size_t ListTail(List_t* list)
{
    if(!list) return 0;

    return list->elements[0].previous;
}

ListErr ListAddFront(List_t* list, list_type value)
{
    if(!list) return LIST_NULL;

    return ListAddAfter(list, 0, value);
}

ListErr ListAddBack(List_t* list, list_type value)
{
    if(!list) return LIST_NULL;

    return ListAddAfter(list, ListTail(list), value);
}

list_type ListGetOnIndex(List_t* list, int index)
{
    if(!list                               
    || index <= 0                      
    || index > (int)list->list_capacity
    || list->elements[index].previous < 0)  return 0;

    return list->elements[index].value;
}

ListErr ListSetOnIndex(List_t* list, int index, list_type value)
{
    if(!list)       return LIST_NULL;
    if(index <= 0 
    || index > (int)list->list_capacity 
    || list->elements[index].previous < 0)                          
                    return LIST_INCORRECT_INDEX;

    list->elements[index].value = value;

    return LIST_CORRECT;
}

int ValueEquality(list_type a, list_type b)
{
    list_type c = a - b;

    if(c < 0.00001f && c > -0.00001f) return 1;
    return 0;
}